#include "process_info.h"
#include <dirent.h>
#include <pwd.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include "utils.h"
#include <iomanip>

// Function to get process user
std::string getProcessUser(int pid) {
    std::ifstream statusFile("/proc/" + std::to_string(pid) + "/status");
    if (!statusFile.is_open()) {
        return "Unknown";
    }

    std::string line;
    while (std::getline(statusFile, line)) {
        if (line.find("Uid:") == 0) {
            std::istringstream ss(line.substr(5));
            int uid;
            ss >> uid;
            return getUserNameFromUid(uid);
        }
    }
    return "Unknown";
}

// Function to get process command
std::string getProcessCommand(int pid) {
    std::ifstream commFile("/proc/" + std::to_string(pid) + "/comm");
    if (!commFile.is_open()) {
        return "Unknown";
    }
    std::string command;
    std::getline(commFile, command);
    return command;
}

// Function to get process memory usage
double getProcessMemoryUsage(int pid) {
    std::ifstream statusFile("/proc/" + std::to_string(pid) + "/status");
    if (!statusFile.is_open()) {
        return 0.0;
    }

    std::string line;
    while (std::getline(statusFile, line)) {
        if (line.find("VmRSS:") == 0) {
            std::istringstream ss(line.substr(6));
            long vmRSS;
            ss >> vmRSS;
            return vmRSS / 1024.0;  // Convert from KB to MB
        }
    }
    return 0.0;
}


// Function to get active processes
std::vector<Process> getActiveProcesses() {
    std::vector<Process> processes;

    DIR* dir = opendir("/proc");
    if (dir == nullptr) {
        std::cerr << "Cannot open /proc directory" << std::endl;
        return processes;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (isdigit(entry->d_name[0])) {
            int pid = std::stoi(entry->d_name);

            // Ensure a valid `Process` object is created
            Process process;
            process.pid = pid;  // Assign the pid correctly
            process.user = getProcessUser(pid);
            process.memoryUsage = getProcessMemoryUsage(pid);
            process.command = getProcessCommand(pid);

            processes.push_back(process);
        }
    }

    closedir(dir);
    return processes;
}
