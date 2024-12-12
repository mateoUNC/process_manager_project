/**
 * @file resource_monitor.cpp
 * @brief Implements functions for monitoring system and process resources.
 *
 * This source file contains the implementation of functions responsible for tracking
 * CPU and memory usage of processes, calculating CPU usage percentages, and managing
 * monitoring threads. It interacts with the `/proc` filesystem to gather necessary
 * process information and updates shared data structures in a thread-safe manner.
 * Additionally, it integrates with the Logger to record significant events and errors.
 */

#include "resource_monitor.h"
#include "globals.h"
#include "logger.h" // Include the Logger header
#include "process_display.h"
#include "process_info.h" // For getActiveProcesses()
#include <algorithm>
#include <cctype> // For isdigit()
#include <chrono>
#include <fstream>  // For std::ifstream
#include <iostream> // For std::cout, std::cerr
#include <sstream>  // For std::stringstream
#include <string>   // For std::string
#include <thread>
#include <unistd.h>
#include <unordered_map>
#include <unordered_set>

/**
 * @brief Retrieves the total CPU time from the system.
 *
 * This function reads the `/proc/stat` file to obtain aggregate CPU time across all cores.
 * It parses the first line starting with "cpu" and sums up the various CPU time fields.
 *
 * @return The total CPU time in jiffies, or 0 if the file cannot be read.
 */
long getTotalCpuTime()
{
    std::ifstream statFile("/proc/stat");
    if (!statFile.is_open())
    {
        std::cerr << "Failed to open /proc/stat" << std::endl;
        Logger::getInstance().error("Failed to open /proc/stat file.");
        return 0; // Return a default value
    }

    std::string line;
    std::getline(statFile, line); // Read the first line containing CPU statistics
    std::stringstream ss(line);
    std::string cpu;
    long user, nice, system, idle, iowait, irq, softirq, steal;

    ss >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;

    // Total CPU time includes all the parsed time fields
    return user + nice + system + idle + iowait + irq + softirq + steal;
}

/**
 * @brief Retrieves the total CPU time consumed by a specific process.
 *
 * This function reads the `/proc/[pid]/stat` file to obtain CPU time information
 * for the specified process. It parses the necessary fields to calculate the total
 * CPU time (user time + system time + children user time + children system time).
 *
 * @param pid The Process ID of the target process.
 * @return The total CPU time in jiffies, or 0 if the file cannot be read.
 */
long getProcessTotalTime(int pid)
{
    std::ifstream statFile("/proc/" + std::to_string(pid) + "/stat");
    if (!statFile.is_open())
    {
        std::string errMsg = "Failed to open /proc/" + std::to_string(pid) + "/stat";
        std::cerr << errMsg << std::endl;
        Logger::getInstance().error(errMsg);
        return 0;
    }

    std::string line;
    std::getline(statFile, line); // Read the stat line
    std::stringstream ss(line);
    std::string ignored;
    long utime, stime, cutime, cstime;

    // Skip the first 13 fields to reach utime, stime, cutime, cstime
    for (int i = 0; i < 13; ++i)
        ss >> ignored;
    ss >> utime >> stime >> cutime >> cstime;

    long totalProcessTime = utime + stime + cutime + cstime;
    return totalProcessTime;
}

/**
 * @brief Calculates the CPU usage percentage for a process.
 *
 * This function computes the CPU usage based on the difference in process time and
 * total CPU time between two intervals, adjusted for the number of CPU cores.
 *
 * @param processTimeDelta The difference in process CPU time between two intervals.
 * @param totalCpuTimeDelta The difference in total CPU time between two intervals.
 * @param numCores The number of CPU cores available on the system.
 * @return The CPU usage percentage of the process.
 */
double calculateCpuUsage(long processTimeDelta, long totalCpuTimeDelta, long numCores)
{
    if (totalCpuTimeDelta == 0)
    {
        Logger::getInstance().warning("Total CPU time delta is zero, cannot calculate CPU usage.");
        return 0.0;
    }

    double cpuUsage = ((double)processTimeDelta / (double)totalCpuTimeDelta) * numCores * 100.0;
    return cpuUsage;
}

/**
 * @brief Monitors CPU usage of processes.
 *
 * This function runs in a dedicated thread, periodically calculating and updating
 * the CPU usage of each monitored process. It reads total CPU time and individual
 * process times to compute the CPU usage percentage. The function respects the
 * monitoringActive and monitoringPaused flags to control its execution flow.
 */
void monitorCpu()
{
    Logger::getInstance().info("CPU monitoring thread started.");
    long previousTotalCpuTime = getTotalCpuTime();

    while (monitoringActive.load())
    {
        // Pause monitoring if the flag is set
        while (monitoringPaused.load() && monitoringActive.load())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        if (!monitoringActive.load())
            break; // Exit if monitoring is no longer active

        // Sleep for the specified update frequency before the next check
        std::this_thread::sleep_for(std::chrono::seconds(updateFrequency.load()));

        long totalCpuTime = getTotalCpuTime();
        long totalCpuTimeDelta = totalCpuTime - previousTotalCpuTime;
        previousTotalCpuTime = totalCpuTime;

        auto activeProcesses = getActiveProcesses();

        {
            // Lock the processes map to ensure thread-safe updates
            std::lock_guard<std::mutex> lock(processMutex);
            for (auto& process : activeProcesses)
            {
                long totalProcessTime = getProcessTotalTime(process.pid);
                long processTimeDelta = totalProcessTime - processes[process.pid].prevTotalTime;

                processes[process.pid] = process; // Update the entire Process struct
                processes[process.pid].prevTotalTime = totalProcessTime;
                processes[process.pid].cpuUsage =
                    calculateCpuUsage(processTimeDelta, totalCpuTimeDelta, sysconf(_SC_NPROCESSORS_ONLN));
            }
        }
    }

    Logger::getInstance().info("CPU monitoring thread stopped.");
}

/**
 * @brief Monitors memory usage of processes.
 *
 * This function runs in a dedicated thread, periodically updating the memory usage
 * of each monitored process. It reads the latest memory usage data from the system
 * and updates the shared processes map accordingly. The function respects the
 * monitoringActive and monitoringPaused flags to control its execution flow.
 */
void monitorMemory()
{
    Logger::getInstance().info("Memory monitoring thread started.");
    while (monitoringActive.load())
    {
        // Pause monitoring if the flag is set
        while (monitoringPaused.load() && monitoringActive.load())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        if (!monitoringActive.load())
            break; // Exit if monitoring is no longer active

        // Sleep for the specified update frequency before the next update
        std::this_thread::sleep_for(std::chrono::seconds(updateFrequency.load()));

        auto activeProcesses = getActiveProcesses();

        {
            // Lock the processes map to ensure thread-safe updates
            std::lock_guard<std::mutex> lock(processMutex);
            for (auto& process : activeProcesses)
            {
                processes[process.pid].memoryUsage = process.memoryUsage;
                processes[process.pid].command = process.command;
                processes[process.pid].user = process.user;
            }
        }
    }
    Logger::getInstance().info("Memory monitoring thread stopped.");
}

/**
 * @brief Monitors and displays processes based on current filters and sorting criteria.
 *
 * This function runs in a dedicated thread, periodically fetching the list of active
 * processes, applying any user-defined filters (such as by user, CPU usage, or memory usage),
 * sorting the processes based on the selected criterion (CPU or memory), and displaying the
 * formatted list to the user. It ensures that the display remains updated and reflects the
 * current state of the system's processes. The function respects the monitoringActive and
 * monitoringPaused flags to control its execution flow.
 */
void monitorProcesses()
{
    Logger::getInstance().info("Process display thread started.");
    while (monitoringActive.load())
    {
        // Pause monitoring if the flag is set
        while (monitoringPaused.load() && monitoringActive.load())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        if (!monitoringActive.load())
            break; // Exit if monitoring is no longer active

        // Sleep for the specified update frequency before the next update
        std::this_thread::sleep_for(std::chrono::seconds(updateFrequency.load()));

        std::vector<Process> processesVector;

        {
            // Lock the processes map to ensure thread-safe access
            std::lock_guard<std::mutex> lock(processMutex);
            for (const auto& pair : processes)
            {
                const auto& process = pair.second;

                // Apply user-defined filters
                if (filterCriterion.first == "user" && process.user != filterCriterion.second)
                {
                    continue;
                }
                if (filterCriterion.first == "cpu" && process.cpuUsage <= std::stod(filterCriterion.second))
                {
                    continue;
                }
                if (filterCriterion.first == "memory" && process.memoryUsage <= std::stod(filterCriterion.second))
                {
                    continue;
                }

                processesVector.push_back(process);
            }
        }

        // Sort the processes based on the selected sorting criterion
        if (sortingCriterion == "cpu")
        {
            std::sort(processesVector.begin(), processesVector.end(),
                      [](const Process& a, const Process& b) { return a.cpuUsage > b.cpuUsage; });
        }
        else if (sortingCriterion == "memory")
        {
            std::sort(processesVector.begin(), processesVector.end(),
                      [](const Process& a, const Process& b) { return a.memoryUsage > b.memoryUsage; });
        }

        // Clear the terminal screen and display the updated list of processes
        std::cout << "\033[2J\033[H"; // ANSI escape code to clear the screen
        printProcesses(processesVector);
    }
    Logger::getInstance().info("Process display thread stopped.");
}
