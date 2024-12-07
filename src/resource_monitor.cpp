#include "resource_monitor.h"
#include "process_display.h"
#include "process_info.h"    // Include this to access getActiveProcesses()
#include "globals.h"
#include <unistd.h>
#include <unordered_map>
#include <thread>
#include <chrono>
#include <algorithm>
#include <iostream>     // For std::cout, std::cerr
#include <fstream>      // For std::ifstream
#include <sstream>      // For std::stringstream
#include <string>       // For std::string

long getTotalCpuTime() {
    std::ifstream statFile("/proc/stat");
    if (!statFile.is_open()) {
        std::cerr << "Failed to open /proc/stat" << std::endl;
        return 0;
    }

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
    if (!statFile.is_open()) {
        // Optionally, you can print an error message here
        return 0;
    }

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

double calculateCpuUsage(long processTimeDelta, long totalCpuTimeDelta, long numCores) {
    if (totalCpuTimeDelta == 0) return 0.0;

    // Calculate CPU usage percentage
    double cpuUsage = ((double)processTimeDelta / (double)totalCpuTimeDelta) * numCores * 100.0;

    return cpuUsage;
}

void monitorProcesses() {
    std::unordered_map<int, Process> processes;

    // Initialize previous total CPU time
    long previousTotalCpuTime = getTotalCpuTime();

    // Get the number of CPU cores
    long numCores = sysconf(_SC_NPROCESSORS_ONLN);

    while (monitoringActive.load()) {
        // Sleep for the interval
        std::this_thread::sleep_for(std::chrono::seconds(3));

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
