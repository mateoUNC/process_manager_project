// command_handler.cpp

#include "command_handler.h"
#include "process_control.h"
#include "resource_monitor.h"
#include "process_display.h"
#include "logging.h"
#include "globals.h"
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <atomic>
#include <csignal>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>


std::thread monitoringThread;

const std::vector<std::string> commands = {
    "start_monitor",
    "stop_monitor",
    "pause_monitor",
    "resume_monitor",
    "list_processes",
    "kill",
    "kill_all",
    "filter",
    "sort_by",
    "log",
    "help",
    "clear",
    "exit",
    "quit"
};


char* commandGenerator(const char* text, int state) {
    static size_t listIndex, len;
    if (state == 0) {
        listIndex = 0;
        len = strlen(text);
    }

    while (listIndex < commands.size()) {
        const std::string& command = commands[listIndex++];
        if (command.compare(0, len, text) == 0) {
            return strdup(command.c_str());
        }
    }
    return nullptr;
}

char** commandCompleter(const char* text, int start, int end) {
    // Only autocomplete the first word (command)
    if (start == 0) {
        return rl_completion_matches(text, commandGenerator);
    }
    return nullptr;
}


void handleSigint(int sig) {
    if (monitoringActive.load()) {
        std::cout << "\nStopping monitoring...\n";
        monitoringActive.store(false); // Stop the monitoring thread
        cv.notify_all();              // Notify the condition variable to unblock the thread
        if (monitoringThread.joinable()) {
            monitoringThread.join();  // Wait for the thread to exit
        }
        std::cout << "Monitoring stopped. You can type other commands.\n";
    }
    
    // Always ensure the prompt is displayed
    std::cout << "ProcessManager> ";
    std::cout.flush();
}

void printHelp() {
    std::cout << BOLD << GREEN << "Available Commands:\n" << RESET;

    // List of commands with descriptions
    std::cout << BOLD << CYAN << "  start_monitor" << RESET << " [cpu|memory] "
              << YELLOW << "- Start monitoring processes.\n" << RESET
              << "                     Default sorting is by CPU usage. Use 'memory' to sort by memory.\n";

    std::cout << BOLD << CYAN << "  stop_monitor" << RESET << "             "
              << YELLOW << "- Stop monitoring processes.\n" << RESET;

    std::cout << BOLD << CYAN << "  pause_monitor" << RESET << "            "
              << YELLOW << "- Pause the monitoring process.\n" << RESET;

    std::cout << BOLD << CYAN << "  resume_monitor" << RESET << "           "
              << YELLOW << "- Resume the paused monitoring process.\n" << RESET;

    std::cout << BOLD << CYAN << "  list_processes" << RESET << "           "
              << YELLOW << "- Display the current list of processes.\n" << RESET;

    std::cout << BOLD << CYAN << "  kill <PID>" << RESET << "              "
              << YELLOW << "- Kill the process with the specified PID.\n" << RESET;

    std::cout << BOLD << CYAN << "  kill_all <cpu|user> <value>" << RESET << " "
              << YELLOW << "- Kill processes exceeding a CPU usage threshold or belonging to a user.\n" << RESET;

    std::cout << BOLD << CYAN << "  filter <user|cpu|memory> <value>" << RESET << " "
              << YELLOW << "- Filter processes by user, CPU usage, or memory usage.\n" << RESET;

    std::cout << BOLD << CYAN << "  sort_by <cpu|memory>" << RESET << "      "
              << YELLOW << "- Change the sorting criterion for monitoring.\n" << RESET;

    std::cout << BOLD << CYAN << "  log [filename]" << RESET << "           "
              << YELLOW << "- Log process information to a file. Default file: 'process_log.txt'.\n" << RESET;

    std::cout << BOLD << CYAN << "  clear" << RESET << "                   "
              << YELLOW << "- Clear the terminal screen.\n" << RESET;

    std::cout << BOLD << CYAN << "  help" << RESET << "                    "
              << YELLOW << "- Show this help message.\n" << RESET;

    std::cout << BOLD << CYAN << "  exit, quit" << RESET << "              "
              << YELLOW << "- Exit the application.\n" << RESET;

    std::cout << BOLD << RED << "\nExamples:\n" << RESET;
    std::cout << "  " << GREEN << "start_monitor" << RESET << " cpu\n";
    std::cout << "  " << GREEN << "start_monitor" << RESET << " memory\n";
    std::cout << "  " << GREEN << "kill 1234" << RESET << "\n";
    std::cout << "  " << GREEN << "kill_all cpu 50" << RESET << "\n";
    std::cout << "  " << GREEN << "filter user root" << RESET << "\n";
    std::cout << "  " << GREEN << "sort_by memory" << RESET << "\n";
    std::cout << "  " << GREEN << "log process_log.txt" << RESET << "\n";

    std::cout << BOLD << GREEN << "\nNotes:\n" << RESET;
    std::cout << YELLOW << "  - Use 'start_monitor' without arguments to sort by CPU usage by default.\n";
    std::cout << "  - Press " << BOLD << "Ctrl+C" << RESET << YELLOW
              << " to stop monitoring without exiting the program.\n" << RESET;
}


void startCommandLoop() {
    // Set up the tab completion function
    rl_attempted_completion_function = commandCompleter;

    std::signal(SIGINT, handleSigint);

    std::string input;
    while (true) {
        // Use readline to get user input with a prompt
        char* line = readline("ProcessManager> ");
        if (!line) {
            // User pressed Ctrl+D
            std::cout << "\n";
            break;
        }

        input = std::string(line);
        free(line);

        // Trim whitespace from the input
        input.erase(0, input.find_first_not_of(" \t\n\r\f\v"));
        input.erase(input.find_last_not_of(" \t\n\r\f\v") + 1);

        if (input.empty()) {
            continue;
        }

        // Add the command to the history
        add_history(input.c_str());

        std::istringstream iss(input);
        std::string command;
        iss >> command;

        if (command == "start_monitor") {
            if (!monitoringActive.load()) {
                // Parse sorting criterion
                std::string sortBy = "cpu"; // Default sorting criterion
                if (iss >> sortBy) {
                    if (sortBy != "cpu" && sortBy != "memory") {
                        std::cout << "Invalid argument. Use 'cpu' or 'memory'. Defaulting to 'cpu'.\n";
                        sortBy = "cpu";
                    }
                }
                sortingCriterion = sortBy; // Update the global sorting criterion

                // Start monitoring
                monitoringActive.store(true);

                // Create threads for CPU, memory, and display
                std::thread cpuThread(monitorCpu);
                std::thread memoryThread(monitorMemory);
                std::thread displayThread(monitorProcesses);

                // Detach threads to run independently
                cpuThread.detach();
                memoryThread.detach();
                displayThread.detach();
            } else {
                std::cout << "Monitoring is already active.\n";
            }
        } 

        else if (command == "pause_monitor") {
            if (monitoringActive.load() && !monitoringPaused.load()) {
                monitoringPaused.store(true);
                std::cout << "Monitoring paused.\n";
            } else if (monitoringPaused.load()) {
                std::cout << "Monitoring is already paused.\n";
            } else {
                std::cout << "Monitoring is not active.\n";
            }
            // Ensure prompt is displayed
            std::cout.flush();
        }

        else if (command == "resume_monitor") {
        if (monitoringPaused.load()) {
            monitoringPaused.store(false);
            std::cout << "Monitoring resumed.\n";
        } else if (!monitoringActive.load()) {
            std::cout << "Monitoring is not active. Use 'start_monitor' to begin monitoring.\n";
        } else {
            std::cout << "Monitoring is already running.\n";
        }
        // Ensure prompt is displayed
        std::cout.flush();
    }


        else if (command == "list_processes") {
            std::vector<Process> processesVector;
            {
                std::lock_guard<std::mutex> lock(processMutex);
                for (const auto& pair : processes) {
                    processesVector.push_back(pair.second);
                }
            }
            printProcesses(processesVector);
        }

        else if (command == "kill_all") {
            std::string filterType;
            if (iss >> filterType) {
                if (filterType == "cpu") {
                    double threshold;
                    if (iss >> threshold) {
                        std::cout << "Are you sure you want to terminate all processes with CPU usage above " 
                                << threshold << "%? (y/n): ";
                        char confirmation;
                        std::cin >> confirmation;

                        if (confirmation == 'y' || confirmation == 'Y') {
                            killProcessesByCpu(threshold); // Custom function to kill processes
                        } else {
                            std::cout << "Termination canceled.\n";
                        }
                    } else {
                        std::cout << "Usage: kill_all cpu <threshold>\n";
                    }
                } else if (filterType == "user") {
                    std::string user;
                    if (iss >> user) {
                        std::cout << "Are you sure you want to terminate all processes for user " << user << "? (y/n): ";
                        char confirmation;
                        std::cin >> confirmation;

                        if (confirmation == 'y' || confirmation == 'Y') {
                            killProcessesByUser(user); // Custom function to kill processes
                        } else {
                            std::cout << "Termination canceled.\n";
                        }
                    } else {
                        std::cout << "Usage: kill_all user <username>\n";
                    }
                } else {
                    std::cout << "Invalid criterion. Use 'cpu' or 'user'.\n";
                }
            } else {
                std::cout << "Usage: kill_all <cpu|user> [value]\n";
            }
        }

        else if (command == "sort_by") {
            std::string sortBy;
            if (iss >> sortBy) {
                if (sortBy == "cpu" || sortBy == "memory") {
                    sortingCriterion = sortBy;
                    std::cout << "Sorting criterion updated to: " << sortBy << "\n";
                } else {
                    std::cout << "Invalid sorting criterion. Use 'cpu' or 'memory'.\n";
                }
            } else {
                std::cout << "Usage: sort_by <cpu|memory>\n";
            }
        }
        else if (command == "filter") {
            std::string filterType;
            if (iss >> filterType) {
                if (filterType == "user") {
                    std::string user;
                    if (iss >> user) {
                        killProcessesByUser(user); // Ensure user is passed as a std::string
                    } else {
                        std::cout << "Usage: kill_all user <username>\n";
                    }
                } else if (filterType == "cpu") {
                    double cpuThreshold;
                    if (iss >> cpuThreshold) {
                        filterCriterion = { "cpu", std::to_string(cpuThreshold) };
                        std::cout << "Filtering processes with CPU usage > " << cpuThreshold << "%\n";
                    } else {
                        std::cout << "Usage: filter cpu <threshold>\n";
                    }
                } else if (filterType == "memory") {
                    double memoryThreshold;
                    if (iss >> memoryThreshold) {
                        filterCriterion = { "memory", std::to_string(memoryThreshold) };
                        std::cout << "Filtering processes with memory usage > " << memoryThreshold << " MB\n";
                    } else {
                        std::cout << "Usage: filter memory <threshold>\n";
                    }
                } else {
                    std::cout << "Invalid filter type. Use 'user', 'cpu', or 'memory'.\n";
                }
            } else {
                std::cout << "Usage: filter <user|cpu|memory> [value]\n";
            }
        }


        else if (command == "log") {
            std::string logFile = "process_log.txt"; // Default file
            if (iss >> logFile) {
                startLogging(logFile); // Function to start logging
            } else {
                std::cout << "Logging started. Default file: process_log.txt\n";
                startLogging(logFile);
            }
        }

        else if (command == "stop_monitor") {
            if (monitoringActive.load()) {
                monitoringActive.store(false); // Stop monitoring
                cv.notify_all();              // Notify the condition variable
                if (monitoringThread.joinable()) {
                    monitoringThread.join();  // Wait for the thread to exit
                }
                std::cout << "Monitoring stopped.\n";
            } else {
                std::cout << "Monitoring is not active.\n";
            }
            // Ensure prompt is displayed
            std::cout.flush();
        }
        else if (command == "kill") {
            int pid;
            if (iss >> pid) {
                std::cout << "Are you sure you want to terminate process " << pid << "? (y/n): ";
                char confirmation;
                std::cin >> confirmation;

                if (confirmation == 'y' || confirmation == 'Y') {
                    if (killProcess(pid)) {
                        std::cout << "Process " << pid << " has been terminated.\n";
                    } else {
                        std::cerr << "Failed to terminate process " << pid << ".\n";
                    }
                } else {
                    std::cout << "Termination of process " << pid << " canceled.\n";
                }
            } else {
                std::cerr << "Usage: kill <PID>\n";
            }
        }
        else if (command == "help") {
            printHelp();
        } 
        else if (command == "clear") {
            // Clear the terminal screen
            std::cout << "\033[2J\033[H"; // ANSI escape code for clearing screen
        } 
        else if (command == "exit" || command == "quit") {
            if (monitoringActive.load()) {
                monitoringActive.store(false);
                cv.notify_all();
                if (monitoringThread.joinable()) {
                    monitoringThread.join();
                }
            }
            break;
        } else {
            std::cerr << "Unknown command: " << command << "\n";
            std::cout << "Type 'help' to see available commands.\n";
        }
    }
}
