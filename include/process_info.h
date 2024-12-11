#ifndef PROCESS_INFO_H
#define PROCESS_INFO_H

#include <string>
#include <vector>

// Structure to store process information
struct Process {
    int pid;                     // Process ID
    std::string user;            // User owning the process
    double cpuUsage;             // CPU usage percentage
    double memoryUsage;          // Memory usage in MB
    long prevTotalTime;          // Previous total CPU time of the process
    std::string command;         // Command associated with the process
};

// Function declarations for process information
std::vector<Process> getActiveProcesses(); // Gets a list of all active processes
std::string getProcessUser(int pid);       // Gets the user of a specific process
std::string getProcessCommand(int pid);    // Gets the command of a specific process
double getProcessMemoryUsage(int pid);     // Gets the memory usage of a specific process

#endif // PROCESS_INFO_H
