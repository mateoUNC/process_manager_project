// command_handler.cpp

#include "command_handler.h"
#include "process_control.h"
#include "resource_monitor.h"
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
    "kill",
    "help",
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

    // List of commands
    std::cout << BOLD << CYAN << "  start_monitor" << RESET << " [cpu|memory] "
              << YELLOW << "- Start monitoring processes.\n" << RESET
              << "                     Default sorting is by CPU usage. Use 'memory' to sort by memory.\n";

    std::cout << BOLD << CYAN << "  stop_monitor" << RESET << "             "
              << YELLOW << "- Stop monitoring processes.\n" << RESET;

    std::cout << BOLD << CYAN << "  kill <PID>" << RESET << "              "
              << YELLOW << "- Kill the process with the specified PID.\n" << RESET;

    std::cout << BOLD << CYAN << "  help" << RESET << "                    "
              << YELLOW << "- Show this help message.\n" << RESET;

    std::cout << BOLD << CYAN << "  exit, quit" << RESET << "              "
              << YELLOW << "- Exit the application.\n" << RESET;

    std::cout << BOLD << RED << "\nExamples:\n" << RESET;
    std::cout << "  " << GREEN << "start_monitor" << RESET << " cpu\n";
    std::cout << "  " << GREEN << "start_monitor" << RESET << " memory\n";
    std::cout << "  " << GREEN << "kill 1234" << RESET << "\n";
    std::cout << "  " << GREEN << "stop_monitor" << RESET << "\n";
    std::cout << "  " << GREEN << "exit" << RESET << "\n";

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
                std::string sortBy = "cpu"; // Default sorting criterion
                if (iss >> sortBy) {
                    if (sortBy != "cpu" && sortBy != "memory") {
                        std::cout << "Invalid argument. Use 'cpu' or 'memory'. Defaulting to 'cpu'.\n";
                        sortBy = "cpu";
                    }
                }

                sortingCriterion = sortBy; // Update the global sorting criterion
                monitoringActive.store(true);
                monitoringThread = std::thread(monitorProcesses);
            } else {
                std::cout << "Monitoring is already active.\n";
            }
        } else if (command == "stop_monitor") {
            if (monitoringActive.load()) {
                monitoringActive.store(false);
                cv.notify_all();
                if (monitoringThread.joinable()) {
                    monitoringThread.join();
                }
                std::cout << "Monitoring stopped.\n";
            } else {
                std::cout << "Monitoring is not active.\n";
            }
        } else if (command == "kill") {
            int pid;
            if (iss >> pid) {
                if (killProcess(pid)) {
                    std::cout << "Process " << pid << " has been killed.\n";
                } else {
                    std::cerr << "Failed to kill process " << pid << ".\n";
                }
            } else {
                std::cerr << "Usage: kill <PID>\n";
            }
        } else if (command == "help") {
            printHelp();
        } else if (command == "exit" || command == "quit") {
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
