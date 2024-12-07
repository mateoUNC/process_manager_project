#include "resource_monitor.h"
#include "process_display.h"
#include "process_info.h"    // Include this to access getActiveProcesses()
#include "globals.h"
#include <unistd.h>
#include <unordered_map>
#include <unordered_set>
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
        return 0; // Return a default value
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
        std::cerr << "Failed to open /proc/" << pid << "/stat" << std::endl;
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

    long previousTotalCpuTime = getTotalCpuTime();

    while (monitoringActive.load()) {
        std::this_thread::sleep_for(std::chrono::seconds(3));

        long totalCpuTime = getTotalCpuTime();
        long totalCpuTimeDelta = totalCpuTime - previousTotalCpuTime;

        std::vector<Process> activeProcesses = getActiveProcesses();

        std::unordered_set<int> activePids;
        for (auto process : activeProcesses) {
            activePids.insert(process.pid);

            auto it = processes.find(process.pid);
            if (it != processes.end()) {
                process.prevTotalTime = it->second.prevTotalTime;
            } else {
                process.prevTotalTime = 0;
            }

            long totalProcessTime = getProcessTotalTime(process.pid);
            long processTimeDelta = totalProcessTime - process.prevTotalTime;
            process.prevTotalTime = totalProcessTime;

            process.cpuUsage = calculateCpuUsage(processTimeDelta, totalCpuTimeDelta, sysconf(_SC_NPROCESSORS_ONLN));

            processes[process.pid] = process;
        }

        for (auto it = processes.begin(); it != processes.end();) {
            if (activePids.find(it->first) == activePids.end()) {
                it = processes.erase(it);
            } else {
                ++it;
            }
        }

        previousTotalCpuTime = totalCpuTime;

        std::vector<Process> processesVector;
        for (const auto& pair : processes) {
            processesVector.push_back(pair.second);
        }

        // Sort processes based on the sorting criterion
        if (sortingCriterion == "cpu") {
            std::sort(processesVector.begin(), processesVector.end(), [](const Process& a, const Process& b) {
                return a.cpuUsage > b.cpuUsage;
            });
        } else if (sortingCriterion == "memory") {
            std::sort(processesVector.begin(), processesVector.end(), [](const Process& a, const Process& b) {
                return a.memoryUsage > b.memoryUsage;
            });
        }

        std::cout << "\033[2J\033[H"; // Clear screen
        printProcesses(processesVector);
    }
}
