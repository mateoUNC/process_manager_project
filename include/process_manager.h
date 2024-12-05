// process_manager.h

#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include <vector>
#include <string>
#include <unordered_map>

// Structure to store process information
struct Process {
    int pid;
    std::string user;
    double cpuUsage;     // Percentage
    double memoryUsage;  // In MB
    long prevTotalTime;  // Previous total CPU time of the process
    std::string command; // Command name
};

std::vector<Process> getActiveProcesses();
void printProcesses(const std::vector<Process>& processes);
void monitorProcesses();

#endif // PROCESS_MANAGER_H
