#ifndef PROCESS_INFO_H
#define PROCESS_INFO_H

#include <string>
#include <vector>

// Structure to store process information
struct Process {
    int pid;
    std::string user;
    double cpuUsage;     // Percentage
    double memoryUsage;  // In MB
    long prevTotalTime;  // Previous total CPU time of the process
    std::string command; // Command name
};

// Function declarations
std::vector<Process> getActiveProcesses();
std::string getProcessUser(int pid);
std::string getProcessCommand(int pid);
double getProcessMemoryUsage(int pid);

#endif // PROCESS_INFO_H
