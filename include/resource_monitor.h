/**
 * @file resource_monitor.h
 * @brief Declares functions for monitoring system and process resources.
 *
 * This header file provides function declarations for tracking CPU and memory usage of processes,
 * calculating CPU usage percentages, and managing monitoring threads.
 */

#ifndef RESOURCE_MONITOR_H
#define RESOURCE_MONITOR_H

#include "process_info.h"
#include <vector>

/**
 * @brief Monitors and updates the list of active processes.
 *
 * Continuously scans for active processes, applies filtering and sorting criteria,
 * and updates the global processes map with the latest information.
 */
void monitorProcesses();

/**
 * @brief Monitors CPU usage of processes.
 *
 * Periodically calculates the CPU usage for each monitored process by comparing
 * the current and previous total CPU times. Updates the CPU usage attribute of each process.
 */
void monitorCpu();

/**
 * @brief Monitors memory usage of processes.
 *
 * Periodically updates the memory usage attribute for each monitored process by reading
 * the latest data from the system.
 */
void monitorMemory();

/**
 * @brief Retrieves the total CPU time from the system.
 *
 * Reads the `/proc/stat` file to calculate the aggregate CPU time across all cores.
 *
 * @return The total CPU time in jiffies, or 0 if it cannot be determined.
 */
long getTotalCpuTime();

/**
 * @brief Retrieves the total CPU time consumed by a specific process.
 *
 * Reads the `/proc/[pid]/stat` file to calculate the total CPU time (user + system) consumed by the process.
 *
 * @param pid The Process ID of the target process.
 * @return The total CPU time in jiffies, or 0 if it cannot be determined.
 */
long getProcessTotalTime(int pid);

/**
 * @brief Calculates the CPU usage percentage for a process.
 *
 * Computes the CPU usage based on the difference in process time and total CPU time between two intervals,
 * adjusted for the number of CPU cores.
 *
 * @param processTimeDelta The difference in process CPU time between two intervals.
 * @param totalCpuTimeDelta The difference in total CPU time between two intervals.
 * @param numCores The number of CPU cores available on the system.
 * @return The CPU usage percentage of the process.
 */
double calculateCpuUsage(long processTimeDelta, long totalCpuTimeDelta, long numCores);

#endif // RESOURCE_MONITOR_H
