#include "process_display.h"
#include <iostream>
#include <iomanip>

void printProcesses(const std::vector<Process>& processes) {
    // Print header with column separators
    std::cout << std::setw(8) << "PID" << " | "
              << std::left << std::setw(14) << "User" << " | "
              << std::setw(9) << "CPU (%)" << " | "
              << std::setw(16) << "Memory (MB)" << " | "
              << "Command" << std::endl;

    // Print separator line
    std::cout << std::string(100, '=') << std::endl;

    int count = 0;
    for (const auto& process : processes) {
        if (count >= 30) break;

        // Truncate command if too long
        std::string command = process.command;
        if (command.length() > 35) {
            command = command.substr(0, 32) + "...";
        }

        // Print data row with column separators
        std::cout << std::setw(8) << process.pid << " | "
                  << std::left << std::setw(14) << process.user << " | "
                  << std::setw(8) << std::fixed << std::setprecision(2)
                  << process.cpuUsage << "% | "
                  << std::setw(13) << std::fixed << std::setprecision(2)
                  << process.memoryUsage << " MB | "
                  << command << std::endl;

        count++;
    }
}
