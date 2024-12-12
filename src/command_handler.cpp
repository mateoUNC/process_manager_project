/**
 * @file command_handler.cpp
 * @brief Implements the command handling logic for the Process Manager application.
 *
 * This source file contains the implementation of functions responsible for handling user
 * commands, managing the command loop, processing signals, and interfacing with other modules
 * such as logging, process control, and resource monitoring. It utilizes the Readline library
 * to provide an interactive command-line interface with features like auto-completion and history.
 */

#include "command_handler.h"
#include "globals.h"
#include "logger.h"
#include "process_control.h"
#include "process_display.h"
#include "resource_monitor.h"
#include <atomic>
#include <csignal>
#include <iostream>
#include <readline/history.h>
#include <readline/readline.h>
#include <sstream>
#include <string>
#include <thread>
#include <unistd.h>

// Thread used for monitoring processes
std::thread monitoringThread;

// List of available commands for the command completer
const std::vector<std::string> commands = {
    "start_monitor", "stop_monitor", "pause_monitor", "resume_monitor", "list_processes",  "kill", "kill_all", "filter",
    "sort_by",       "log",          "help",          "clear",          "set_update_freq", "exit", "quit"};

char* commandGenerator(const char* text, int state)
{
    static size_t listIndex, len;
    if (state == 0)
    {
        listIndex = 0;      // Reset index for a new completion attempt
        len = strlen(text); // Length of the current input
    }

    // Iterate through the commands to find matches
    while (listIndex < commands.size())
    {
        const std::string& command = commands[listIndex++];
        if (command.compare(0, len, text) == 0)
        {
            return strdup(command.c_str()); // Return a duplicate of the matching command
        }
    }
    return nullptr; // No more matches
}

char** commandCompleter(const char* text, int start, int end)
{
    // Only autocomplete the first word (command)
    if (start == 0)
    {
        return rl_completion_matches(text, commandGenerator);
    }
    return nullptr;
}

void handleSigint(int sig)
{
    if (monitoringActive.load())
    {
        std::cout << "\nStopping monitoring...\n";
        monitoringActive.store(false); // Signal monitoring threads to stop
        cv.notify_all();               // Notify condition variable to unblock threads

        if (monitoringThread.joinable())
        {
            monitoringThread.join(); // Wait for the monitoring thread to finish
        }
        std::cout << "Monitoring stopped. You can type other commands.\n";
    }

    // Always ensure the prompt is displayed after handling SIGINT
    std::cout << "ProcessManager> ";
    std::cout.flush();
}

void printHelp()
{
    std::cout << BOLD << GREEN << "Available Commands:\n" << RESET;

    // List of commands with descriptions
    std::cout << BOLD << CYAN << "  start_monitor" << RESET << " [cpu|memory] " << YELLOW
              << "- Start monitoring processes.\n"
              << RESET << "                     Default sorting is by CPU usage. Use 'memory' to sort by memory.\n";

    std::cout << BOLD << CYAN << "  stop_monitor" << RESET << "             " << YELLOW
              << "- Stop monitoring processes.\n"
              << RESET;

    std::cout << BOLD << CYAN << "  pause_monitor" << RESET << "            " << YELLOW
              << "- Pause the monitoring process.\n"
              << RESET;

    std::cout << BOLD << CYAN << "  resume_monitor" << RESET << "           " << YELLOW
              << "- Resume the paused monitoring process.\n"
              << RESET;

    std::cout << BOLD << CYAN << "  list_processes" << RESET << "           " << YELLOW
              << "- Display the current list of processes.\n"
              << RESET;

    std::cout << BOLD << CYAN << "  kill <PID>" << RESET << "              " << YELLOW
              << "- Kill the process with the specified PID.\n"
              << RESET;

    std::cout << BOLD << CYAN << "  kill_all <cpu|user> <value>" << RESET << " " << YELLOW
              << "- Kill processes exceeding a CPU usage threshold or belonging to a user.\n"
              << RESET;

    std::cout << BOLD << CYAN << "  filter <user|cpu|memory> <value>" << RESET << " " << YELLOW
              << "- Filter processes by user, CPU usage, or memory usage.\n"
              << RESET;

    std::cout << BOLD << CYAN << "  sort_by <cpu|memory>" << RESET << "      " << YELLOW
              << "- Change the sorting criterion for monitoring.\n"
              << RESET;

    std::cout << BOLD << CYAN << "  log [filename]" << RESET << "           " << YELLOW
              << "- Log process information to a file. Default file: 'process_log.txt'.\n"
              << RESET;

    std::cout << BOLD << CYAN << "  set_update_freq <seconds>" << RESET << "  " << YELLOW
              << "- Change the update frequency for resource monitoring.\n"
              << RESET << "                     For example, 'set_update_freq 10' updates data every 10 seconds.\n";

    std::cout << BOLD << CYAN << "  clear" << RESET << "                   " << YELLOW
              << "- Clear the terminal screen.\n"
              << RESET;

    std::cout << BOLD << CYAN << "  help" << RESET << "                    " << YELLOW << "- Show this help message.\n"
              << RESET;

    std::cout << BOLD << CYAN << "  exit, quit" << RESET << "              " << YELLOW << "- Exit the application.\n"
              << RESET;

    // Provide examples of command usage
    std::cout << BOLD << RED << "\nExamples:\n" << RESET;
    std::cout << "  " << GREEN << "start_monitor" << RESET << " cpu\n";
    std::cout << "  " << GREEN << "start_monitor" << RESET << " memory\n";
    std::cout << "  " << GREEN << "kill 1234" << RESET << "\n";
    std::cout << "  " << GREEN << "kill_all cpu 50" << RESET << "\n";
    std::cout << "  " << GREEN << "filter user root" << RESET << "\n";
    std::cout << "  " << GREEN << "sort_by memory" << RESET << "\n";
    std::cout << "  " << GREEN << "log process_log.txt" << RESET << "\n";
    std::cout << "  " << GREEN << "set_update_freq 10" << RESET << "\n";

    // Provide additional notes for clarification
    std::cout << BOLD << GREEN << "\nNotes:\n" << RESET;
    std::cout << YELLOW << "  - Use 'start_monitor' without arguments to sort by CPU usage by default.\n";
    std::cout << "  - Press " << BOLD << "Ctrl+C" << RESET << YELLOW
              << " to stop monitoring without exiting the program.\n"
              << RESET;
}

void startCommandLoop()
{
    // Set up the tab completion function for Readline
    rl_attempted_completion_function = commandCompleter;

    // Register the SIGINT signal handler for graceful shutdown on Ctrl+C
    std::signal(SIGINT, handleSigint);

    std::string input;
    while (true)
    {
        // Use readline to get user input with a prompt
        char* line = readline("ProcessManager> ");
        if (!line)
        {
            // User pressed Ctrl+D to exit
            std::cout << "\n";
            break;
        }

        input = std::string(line);
        free(line);

        // Trim leading and trailing whitespace from the input
        input.erase(0, input.find_first_not_of(" \t\n\r\f\v"));
        input.erase(input.find_last_not_of(" \t\n\r\f\v") + 1);

        if (input.empty())
        {
            continue; // Ignore empty inputs
        }

        // Add the command to the history for future reference
        add_history(input.c_str());

        // Parse the command and its arguments using a string stream
        std::istringstream iss(input);
        std::string command;
        iss >> command;

        // Handle the "start_monitor" command
        if (command == "start_monitor")
        {
            if (!monitoringActive.load())
            {
                // Parse sorting criterion if provided (default is "cpu")
                std::string sortBy = "cpu";
                if (iss >> sortBy)
                {
                    if (sortBy != "cpu" && sortBy != "memory")
                    {
                        std::cout << "Invalid argument. Use 'cpu' or 'memory'. Defaulting to 'cpu'.\n";
                        sortBy = "cpu";
                    }
                }
                sortingCriterion = sortBy; // Update the global sorting criterion

                // Start monitoring by setting the active flag
                monitoringActive.store(true);

                // Create separate threads for CPU monitoring, memory monitoring, and process display
                std::thread cpuThread(monitorCpu);
                std::thread memoryThread(monitorMemory);
                std::thread displayThread(monitorProcesses);

                // Detach threads to allow them to run independently
                cpuThread.detach();
                memoryThread.detach();
                displayThread.detach();

                std::cout << "Monitoring started with sorting by " << sortBy << ".\n";
                Logger::getInstance().info("User started monitoring with sorting by " + sortBy + ".");
            }
            else
            {
                std::cout << "Monitoring is already active.\n";
            }
        }

        // Handle the "pause_monitor" command
        else if (command == "pause_monitor")
        {
            if (monitoringActive.load() && !monitoringPaused.load())
            {
                monitoringPaused.store(true);
                std::cout << "Monitoring paused.\n";
                Logger::getInstance().info("User paused monitoring.");
            }
            else if (monitoringPaused.load())
            {
                std::cout << "Monitoring is already paused.\n";
                Logger::getInstance().warning("User attempted to pause monitoring when it is already paused.");
            }
            else
            {
                std::cout << "Monitoring is not active.\n";
                Logger::getInstance().warning("User attempted to pause monitoring when it is not active.");
            }
            // Ensure prompt is displayed immediately
            std::cout.flush();
        }

        // Handle the "resume_monitor" command
        else if (command == "resume_monitor")
        {
            if (monitoringPaused.load())
            {
                monitoringPaused.store(false);
                std::cout << "Monitoring resumed.\n";
                Logger::getInstance().info("User resumed monitoring.");
            }
            else if (!monitoringActive.load())
            {
                std::cout << "Monitoring is not active. Use 'start_monitor' to begin monitoring.\n";
                Logger::getInstance().warning("User attempted to resume monitoring when it is not active.");
            }
            else
            {
                std::cout << "Monitoring is already running.\n";
                Logger::getInstance().warning("User attempted to resume monitoring when it is already running.");
            }
            // Ensure prompt is displayed immediately
            std::cout.flush();
        }

        // Handle the "list_processes" command
        else if (command == "list_processes")
        {
            std::vector<Process> processesVector;
            {
                std::lock_guard<std::mutex> lock(processMutex);
                for (const auto& pair : processes)
                {
                    processesVector.push_back(pair.second);
                }
            }
            printProcesses(processesVector);
            Logger::getInstance().info("User listed all processes.");
        }

        // Handle the "kill_all" command
        else if (command == "kill_all")
        {
            std::string filterType;
            if (iss >> filterType)
            {
                if (filterType == "cpu")
                {
                    double threshold;
                    if (iss >> threshold)
                    {
                        std::cout << "Are you sure you want to terminate all processes with CPU usage above "
                                  << threshold << "%? (y/n): ";
                        char confirmation;
                        std::cin >> confirmation;

                        if (confirmation == 'y' || confirmation == 'Y')
                        {
                            if (killProcessesByCpu(threshold))
                            {
                                std::cout << "Processes exceeding " << threshold
                                          << "% CPU usage have been terminated.\n";
                                Logger::getInstance().info("User killed all processes with CPU usage above " +
                                                           std::to_string(threshold) + "%.");
                            }
                            else
                            {
                                std::cout << "No processes found exceeding the CPU usage threshold.\n";
                                Logger::getInstance().info(
                                    "User attempted to kill processes by CPU usage, but none matched the threshold.");
                            }
                        }
                        else
                        {
                            std::cout << "Termination canceled.\n";
                            Logger::getInstance().info("User canceled termination of processes by CPU usage.");
                        }
                    }
                    else
                    {
                        std::cout << "Usage: kill_all cpu <threshold>\n";
                        Logger::getInstance().warning("User provided invalid arguments for kill_all cpu command.");
                    }
                }
                else if (filterType == "user")
                {
                    std::string user;
                    if (iss >> user)
                    {
                        std::cout << "Are you sure you want to terminate all processes for user " << user
                                  << "? (y/n): ";
                        char confirmation;
                        std::cin >> confirmation;

                        if (confirmation == 'y' || confirmation == 'Y')
                        {
                            if (killProcessesByUser(user))
                            {
                                std::cout << "All processes for user " << user << " have been terminated.\n";
                                Logger::getInstance().info("User killed all processes belonging to user: " + user +
                                                           ".");
                            }
                            else
                            {
                                std::cout << "No processes found for user: " << user << "\n";
                                Logger::getInstance().info(
                                    "User attempted to kill processes by user, but none were found for user: " + user +
                                    ".");
                            }
                        }
                        else
                        {
                            std::cout << "Termination canceled.\n";
                            Logger::getInstance().info("User canceled termination of processes by user: " + user + ".");
                        }
                    }
                    else
                    {
                        std::cout << "Usage: kill_all user <username>\n";
                        Logger::getInstance().warning("User provided invalid arguments for kill_all user command.");
                    }
                }
                else
                {
                    std::cout << "Invalid criterion. Use 'cpu' or 'user'.\n";
                    Logger::getInstance().warning("User provided invalid filter type for kill_all command: " +
                                                  filterType);
                }
            }
            else
            {
                std::cout << "Usage: kill_all <cpu|user> [value]\n";
                Logger::getInstance().warning("User attempted to use kill_all command without sufficient arguments.");
            }
        }

        // Handle the "sort_by" command
        else if (command == "sort_by")
        {
            std::string sortBy;
            if (iss >> sortBy)
            {
                if (sortBy == "cpu" || sortBy == "memory")
                {
                    sortingCriterion = sortBy;
                    std::cout << "Sorting criterion updated to: " << sortBy << "\n";
                    Logger::getInstance().info("User changed sorting criterion to: " + sortBy + ".");
                }
                else
                {
                    std::cout << "Invalid sorting criterion. Use 'cpu' or 'memory'.\n";
                    Logger::getInstance().warning("User provided invalid sorting criterion: " + sortBy + ".");
                }
            }
            else
            {
                std::cout << "Usage: sort_by <cpu|memory>\n";
                Logger::getInstance().warning("User attempted to use sort_by command without specifying a criterion.");
            }
        }

        // Handle the "filter" command
        else if (command == "filter")
        {
            std::string filterType;
            if (iss >> filterType)
            {
                if (filterType == "user")
                {
                    std::string user;
                    if (iss >> user)
                    {
                        filterCriterion = {"user", user};
                        Logger::getInstance().info("User applied filter by user: " + user);
                        std::cout << "Filter applied by user: " << user << "\n";
                    }
                    else
                    {
                        std::cout << "Usage: filter user <username>\n";
                        Logger::getInstance().warning(
                            "User attempted to use filter user command without specifying a username.");
                    }
                }
                else if (filterType == "cpu")
                {
                    double cpuThreshold;
                    if (iss >> cpuThreshold)
                    {
                        // Format the threshold to remove trailing zeros if it's an integer
                        std::ostringstream oss;
                        if (cpuThreshold == static_cast<int>(cpuThreshold))
                        {
                            oss << static_cast<int>(cpuThreshold);
                        }
                        else
                        {
                            oss << cpuThreshold;
                        }
                        filterCriterion = {"cpu", oss.str()};
                        Logger::getInstance().info("User applied CPU filter: > " + oss.str() + "%");
                        std::cout << "CPU filter applied: > " << oss.str() << "%\n";
                    }
                    else
                    {
                        std::cout << "Usage: filter cpu <threshold>\n";
                        Logger::getInstance().warning(
                            "User attempted to use filter cpu command without specifying a threshold.");
                    }
                }
                else if (filterType == "memory")
                {
                    double memoryThreshold;
                    if (iss >> memoryThreshold)
                    {
                        // Format the threshold to remove trailing zeros if it's an integer
                        std::ostringstream oss;
                        if (memoryThreshold == static_cast<int>(memoryThreshold))
                        {
                            oss << static_cast<int>(memoryThreshold);
                        }
                        else
                        {
                            oss << memoryThreshold;
                        }
                        filterCriterion = {"memory", oss.str()};
                        Logger::getInstance().info("User applied Memory filter: > " + oss.str() + " MB");
                        std::cout << "Memory filter applied: > " << oss.str() << " MB\n";
                    }
                    else
                    {
                        std::cout << "Usage: filter memory <threshold>\n";
                        Logger::getInstance().warning(
                            "User attempted to use filter memory command without specifying a threshold.");
                    }
                }
                else
                {
                    std::cout << "Invalid filter type. Use 'user', 'cpu', or 'memory'.\n";
                    Logger::getInstance().warning("User provided invalid filter type: " + filterType + ".");
                }
            }
            else
            {
                std::cout << "Usage: filter <user|cpu|memory> [value]\n";
                Logger::getInstance().warning("User attempted to use filter command without sufficient arguments.");
            }
        }

        // Handle the "log" command
        else if (command == "log")
        {
            std::string logFile = "process_log.txt"; // Default log file
            if (iss >> logFile)
            {
                if (!Logger::getInstance().start(logFile))
                {
                    std::cerr << "Failed to start logger on file: " << logFile << "\n";
                    Logger::getInstance().error("Failed to start logger on file: " + logFile + ".");
                }
                else
                {
                    std::cout << "Logging started on file: " << logFile << "\n";
                    Logger::getInstance().info("User started logging on file: " + logFile + ".");
                }
            }
            else
            {
                // Start logging with the default file
                if (!Logger::getInstance().start(logFile))
                {
                    std::cerr << "Failed to start logger on file: " << logFile << "\n";
                    Logger::getInstance().error("Failed to start logger on default file: " + logFile + ".");
                }
                else
                {
                    std::cout << "Logging started. Default file: process_log.txt\n";
                    Logger::getInstance().info("User started logging on default file: process_log.txt.");
                }
            }
        }

        // Handle the "stop_monitor" command
        else if (command == "stop_monitor")
        {
            if (monitoringActive.load())
            {
                Logger::getInstance().info("User stopped monitoring.");

                monitoringActive.store(false); // Signal monitoring threads to stop
                cv.notify_all();               // Notify condition variable to unblock threads

                if (monitoringThread.joinable())
                {
                    monitoringThread.join(); // Wait for the monitoring thread to finish
                }
                std::cout << "Monitoring stopped.\n";
            }
            else
            {
                std::cout << "Monitoring is not active.\n";
                Logger::getInstance().warning("User attempted to stop monitoring when it was not active.");
            }
            // Ensure prompt is displayed immediately
            std::cout.flush();
        }

        // Handle the "kill" command
        else if (command == "kill")
        {
            int pid;
            if (iss >> pid)
            {
                std::cout << "Are you sure you want to terminate process " << pid << "? (y/n): ";
                char confirmation;
                std::cin >> confirmation;

                if (confirmation == 'y' || confirmation == 'Y')
                {
                    if (killProcess(pid))
                    {
                        std::cout << "Process " << pid << " has been terminated.\n";
                        Logger::getInstance().info("User terminated process PID: " + std::to_string(pid) + ".");
                    }
                    else
                    {
                        std::cerr << "Failed to terminate process " << pid << ".\n";
                        Logger::getInstance().error("Failed to terminate process PID: " + std::to_string(pid) + ".");
                    }
                }
                else
                {
                    std::cout << "Termination of process " << pid << " canceled.\n";
                    Logger::getInstance().info("User canceled termination of process PID: " + std::to_string(pid) +
                                               ".");
                }
            }
            else
            {
                std::cerr << "Usage: kill <PID>\n";
                Logger::getInstance().warning("User attempted to use kill command without specifying a PID.");
            }
        }

        // Handle the "help" command
        else if (command == "help")
        {
            printHelp();
        }

        // Handle the "clear" command
        else if (command == "clear")
        {
            // Clear the terminal screen using ANSI escape codes
            std::cout << "\033[2J\033[H";
        }

        // Handle the "set_update_freq" command
        else if (command == "set_update_freq")
        {
            int newFreq;
            if (iss >> newFreq)
            {
                if (newFreq <= 0)
                {
                    std::cout << "Invalid frequency. Please provide a positive integer value.\n";
                }
                else
                {
                    updateFrequency.store(newFreq);
                    std::cout << "Update frequency set to " << newFreq << " seconds.\n";
                    Logger::getInstance().info("User changed update frequency to " + std::to_string(newFreq) +
                                               " seconds.");
                }
            }
            else
            {
                std::cout << "Usage: set_update_freq <seconds>\n";
            }
        }

        // Handle the "exit" and "quit" commands
        else if (command == "exit" || command == "quit")
        {
            // Stop monitoring if it is active
            if (monitoringActive.load())
            {
                monitoringActive.store(false);
                cv.notify_all();
                if (monitoringThread.joinable())
                {
                    monitoringThread.join();
                }
            }
            Logger::getInstance().info("User exited the application.");
            // Stop the logger to ensure all logs are flushed and the file is closed
            Logger::getInstance().stop();
            break; // Exit the command loop
        }

        // Handle unknown commands
        else
        {
            std::cerr << "Unknown command: " << command << "\n";
            std::cout << "Type 'help' to see available commands.\n";
        }
    }
}
