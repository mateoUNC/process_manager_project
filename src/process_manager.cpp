#include "process_manager.h"
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <sstream>
#include <unistd.h>
#include <unordered_map>
#include <chrono>
#include <sys/stat.h>
#include <iomanip>
#include <algorithm>
#include <pwd.h>

std::string getUserNameFromUid(int uid) {
    struct passwd *pw = getpwuid(uid);
    if (pw) {
        return std::string(pw->pw_name);
    }
    return "Unknown";
}

std::string getProcessCommand(int pid) {
    std::ifstream commFile("/proc/" + std::to_string(pid) + "/comm");
    if (!commFile.is_open()) {
        return "Unknown";
    }
    std::string command;
    std::getline(commFile, command);
    return command;
}

std::string getProcessUser(int pid) {
    std::ifstream statusFile("/proc/" + std::to_string(pid) + "/status");
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

long getTotalCpuTime() {
    std::ifstream statFile("/proc/stat");
    std::string line;
    std::getline(statFile, line);
    std::stringstream ss(line);
    std::string cpu;
    long user, nice, system, idle, iowait, irq, softirq, steal;

    ss >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;

    // Total CPU time includes all times
    return user + nice + system + idle + iowait + irq + softirq + steal;
}

long getProcessTotalTime(int pid) {
    std::ifstream statFile("/proc/" + std::to_string(pid) + "/stat");
    if (!statFile.is_open()) return 0;

    std::string line;
    std::getline(statFile, line);
    std::stringstream ss(line);
    std::string ignored;
    long utime, stime, cutime, cstime;

    // Skip fields until utime
    for (int i = 0; i < 13; ++i) ss >> ignored;
    ss >> utime >> stime >> cutime >> cstime;

    long totalProcessTime = utime + stime + cutime + cstime;

    return totalProcessTime;
}


double getProcessMemoryUsage(int pid) {
    std::ifstream statusFile("/proc/" + std::to_string(pid) + "/status");
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

double calculateCpuUsage(long processTimeDelta, long totalCpuTimeDelta, long numCores) {
    if (totalCpuTimeDelta == 0) return 0.0;

    // Calculate CPU usage percentage
    double cpuUsage = ((double)processTimeDelta / (double)totalCpuTimeDelta) * numCores * 100.0;

    return cpuUsage;
}


// process_manager.cpp

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

            Process process;
            process.pid = pid;
            process.user = getProcessUser(pid);
            process.memoryUsage = getProcessMemoryUsage(pid);
            process.command = getProcessCommand(pid);  // Get the command name

            processes.push_back(process);
        }
    }

    closedir(dir);

    return processes;
}


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




void monitorProcesses() {
    std::unordered_map<int, Process> processes;

    // Initialize previous total CPU time
    long previousTotalCpuTime = getTotalCpuTime();

    // Get the number of CPU cores
    long numCores = sysconf(_SC_NPROCESSORS_ONLN);

    while (true) {
        // Sleep for the interval
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // Get total CPU time at the end
        long totalCpuTime = getTotalCpuTime();
        long totalCpuTimeDelta = totalCpuTime - previousTotalCpuTime;

        // Read active processes
        std::vector<Process> activeProcesses = getActiveProcesses();

        // Update CPU usage for each process
        for (auto& process : activeProcesses) {
            // If the process is already in the map, use its prevTotalTime
            auto it = processes.find(process.pid);
            if (it != processes.end()) {
                process.prevTotalTime = it->second.prevTotalTime;
            } else {
                // First time seeing this process; initialize prevTotalTime
                process.prevTotalTime = 0;
            }

            // Get process CPU times
            long totalProcessTime = getProcessTotalTime(process.pid);
            long processTimeDelta = totalProcessTime - process.prevTotalTime;
            process.prevTotalTime = totalProcessTime;

            // Calculate CPU usage
            process.cpuUsage = calculateCpuUsage(processTimeDelta, totalCpuTimeDelta, numCores);

            // Update the processes map
            processes[process.pid] = process;
        }

        // Update previous total CPU time
        previousTotalCpuTime = totalCpuTime;

        // Convert map to vector for sorting
        std::vector<Process> processesVector;
        for (const auto& pair : processes) {
            processesVector.push_back(pair.second);
        }

        // Sort processes by CPU usage
        std::sort(processesVector.begin(), processesVector.end(), [](const Process& a, const Process& b) {
            return a.cpuUsage > b.cpuUsage;
        });

        // Clear the screen
        system("clear");

        // Print the processes
        printProcesses(processesVector);

        // Remove processes that are no longer active
        for (auto it = processes.begin(); it != processes.end();) {
            if (std::find_if(activeProcesses.begin(), activeProcesses.end(), [&](const Process& p) {
                return p.pid == it->first;
            }) == activeProcesses.end()) {
                it = processes.erase(it);
            } else {
                ++it;
            }
        }
    }
}
