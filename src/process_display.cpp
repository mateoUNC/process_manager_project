/**
 * @file process_display.cpp
 * @brief Implements functions for displaying process information to the user.
 *
 * This source file contains the implementation of the `printProcesses` function, which
 * formats and outputs a list of processes to the console in a tabular format. It applies
 * color-coding based on CPU usage to enhance readability and highlights critical information
 * about each process, such as PID, user, CPU usage, memory usage, and the associated command.
 * The function ensures that the output remains organized and does not overwhelm the user by
 * limiting the display to a maximum of 30 processes at a time.
 */

#include "process_display.h"
#include <iomanip>
#include <iostream>

// Define ANSI color codes for styling the console output
#define RESET "\033[0m"
#define RED "\033[31m"
#define YELLOW "\033[33m"
#define GREEN "\033[32m"

/**
 * @brief Displays a formatted list of processes in the console.
 *
 * The `printProcesses` function takes a vector of `Process` structures and outputs
 * their details in a neatly formatted table. It applies color-coding to the CPU usage
 * column to indicate the intensity of resource consumption:
 * - **Red**: High CPU usage (>20%)
 * - **Yellow**: Moderate CPU usage (>10%)
 * - **Green**: Low CPU usage (≤10%)
 *
 * The function truncates the command string if it exceeds 35 characters to maintain
 * the table's alignment and readability. It also limits the display to the first 30
 * processes to prevent excessive output.
 *
 * @param processes A vector of `Process` structures containing information about active processes.
 */
void printProcesses(const std::vector<Process>& processes)
{
    // Print the table header with column names
    std::cout << std::setw(8) << "PID"
              << " | " << std::left << std::setw(14) << "User"
              << " | " << std::setw(9) << "CPU (%)"
              << " | " << std::setw(16) << "Memory (MB)"
              << " | "
              << "Command" << std::endl;

    // Print a separator line to distinguish the header from the process entries
    std::cout << std::string(100, '=') << std::endl;

    int count = 0; // Counter to limit the number of displayed processes
    for (const auto& process : processes)
    {
        if (count >= 30)
            break; // Limit the display to the first 30 processes

        // Determine the color based on CPU usage
        std::string color;
        if (process.cpuUsage > 20.0)
        {
            color = RED; // High CPU usage
        }
        else if (process.cpuUsage > 10.0)
        {
            color = YELLOW; // Moderate CPU usage
        }
        else
        {
            color = GREEN; // Low CPU usage
        }

        // Truncate the command string if it exceeds 35 characters to maintain table alignment
        std::string command = process.command;
        if (command.length() > 35)
        {
            command = command.substr(0, 32) + "...";
        }

        // Print the process information row with appropriate formatting and color-coding
        std::cout << std::setw(8) << process.pid << " | " << std::left << std::setw(14) << process.user << " | "
                  << color << std::setw(8) << std::fixed << std::setprecision(2) << process.cpuUsage << "%" << RESET
                  << " | " << std::setw(13) << std::fixed << std::setprecision(2) << process.memoryUsage << " MB | "
                  << command << std::endl;

        count++; // Increment the counter after displaying a process
    }
}
