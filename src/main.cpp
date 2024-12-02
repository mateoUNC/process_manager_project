//maion

#include <iostream>
#include <vector>
#include <iomanip>  // For std::setw, std::fixed, std::setprecision
#include "process_manager.h"
// Assuming you have your Process structure and getActiveProcesses() function already
void printProcesses(const std::vector<Process>& processes) {
    // Print the table header
    std::cout << std::left
              << std::setw(10) << "PID"
              << std::setw(30) << "Process Name"
              << std::setw(20) << "CPU Usage (s)"
              << std::setw(20) << "Memory Usage (MB)"
              << std::endl;
    std::cout << "------------------------------------------------------------" << std::endl;

    // Print each process's details
    for (const auto& process : processes) {
        std::cout << std::left
                  << std::setw(10) << process.pid
                  << std::setw(30) << process.name
                  << std::setw(20) << std::fixed << std::setprecision(2) << process.cpuUsage
                  << std::setw(20) << std::fixed << std::setprecision(2) << process.memoryUsage
                  << std::endl;
    }
}

int main() {
    // Get active processes
    std::vector<Process> processes = getActiveProcesses();
    
    // Print the processes in a nice table format
    printProcesses(processes);

    return 0;
}
