#include "resource_monitor.h"
#include "process_display.h"
#include "process_info.h"    // For getActiveProcesses()
#include "globals.h"
#include "logger.h"          // Include the Logger header
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
        Logger::getInstance().error("Failed to open /proc/stat file.");
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
        std::string errMsg = "Failed to open /proc/" + std::to_string(pid) + "/stat";
        std::cerr << errMsg << std::endl;
        Logger::getInstance().error(errMsg);
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
    if (totalCpuTimeDelta == 0) {
        Logger::getInstance().warning("Total CPU time delta is zero, cannot calculate CPU usage.");
        return 0.0;
    }

    double cpuUsage = ((double)processTimeDelta / (double)totalCpuTimeDelta) * numCores * 100.0;
    return cpuUsage;
}

void monitorCpu() {
    Logger::getInstance().info("CPU monitoring thread started.");
    long previousTotalCpuTime = getTotalCpuTime();

    while (monitoringActive.load()) {
        // Pause if monitoring is paused
        while (monitoringPaused.load() && monitoringActive.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        if (!monitoringActive.load()) break;
        std::this_thread::sleep_for(std::chrono::seconds(3));

        long totalCpuTime = getTotalCpuTime();
        long totalCpuTimeDelta = totalCpuTime - previousTotalCpuTime;
        previousTotalCpuTime = totalCpuTime;

        auto activeProcesses = getActiveProcesses();

        {
            std::lock_guard<std::mutex> lock(processMutex);
            for (auto& process : activeProcesses) {
                long totalProcessTime = getProcessTotalTime(process.pid);
                long processTimeDelta = totalProcessTime - processes[process.pid].prevTotalTime;

                processes[process.pid] = process;  // Update the entire Process struct
                processes[process.pid].prevTotalTime = totalProcessTime;
                processes[process.pid].cpuUsage = calculateCpuUsage(processTimeDelta, totalCpuTimeDelta, sysconf(_SC_NPROCESSORS_ONLN));
            }
        }
    }

    Logger::getInstance().info("CPU monitoring thread stopped.");
}

void monitorMemory() {
    Logger::getInstance().info("Memory monitoring thread started.");
    while (monitoringActive.load()) {
        // Pause if monitoring is paused
        while (monitoringPaused.load() && monitoringActive.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        if (!monitoringActive.load()) break;
        std::this_thread::sleep_for(std::chrono::seconds(3));

        auto activeProcesses = getActiveProcesses();

        // Update memory usage for all processes
        {
            std::lock_guard<std::mutex> lock(processMutex);
            for (auto& process : activeProcesses) {
                processes[process.pid].memoryUsage = process.memoryUsage;
                processes[process.pid].command = process.command;
                processes[process.pid].user = process.user;
            }
        }
    }
    Logger::getInstance().info("Memory monitoring thread stopped.");
}

void monitorProcesses() {
    Logger::getInstance().info("Process display thread started.");
    while (monitoringActive.load()) {
        // Pause if monitoring is paused
        while (monitoringPaused.load() && monitoringActive.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        if (!monitoringActive.load()) break;
        std::this_thread::sleep_for(std::chrono::seconds(3));

        std::vector<Process> processesVector;

        {
            std::lock_guard<std::mutex> lock(processMutex);
            for (const auto& pair : processes) {
                const auto& process = pair.second;

                // Apply filter
                if (filterCriterion.first == "user" && process.user != filterCriterion.second) {
                    continue; 
                }
                if (filterCriterion.first == "cpu" && process.cpuUsage <= std::stod(filterCriterion.second)) {
                    continue;
                }
                if (filterCriterion.first == "memory" && process.memoryUsage <= std::stod(filterCriterion.second)) {
                    continue;
                }

                processesVector.push_back(process);
            }
        }

        // Sort by the selected criterion
        if (sortingCriterion == "cpu") {
            std::sort(processesVector.begin(), processesVector.end(), [](const Process& a, const Process& b) {
                return a.cpuUsage > b.cpuUsage;
            });
        } else if (sortingCriterion == "memory") {
            std::sort(processesVector.begin(), processesVector.end(), [](const Process& a, const Process& b) {
                return a.memoryUsage > b.memoryUsage;
            });
        }

        // Clear the screen and display the filtered, sorted list
        std::cout << "\033[2J\033[H"; // Clear screen
        printProcesses(processesVector);
    }
    Logger::getInstance().info("Process display thread stopped.");
}
