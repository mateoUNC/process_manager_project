#ifndef RESOURCE_MONITOR_H
#define RESOURCE_MONITOR_H

#include "process_info.h"
#include <vector>

// Function declarations for monitoring processes
void monitorProcesses(); // Monitors and updates the process list
void monitorCpu();       // Monitors CPU usage
void monitorMemory();    // Monitors memory usage

// Helper functions for process monitoring
long getTotalCpuTime();            // Gets total CPU time from /proc/stat
long getProcessTotalTime(int pid); // Gets total CPU time of a specific process
double calculateCpuUsage(long processTimeDelta, long totalCpuTimeDelta,
                         long numCores); // Calculates CPU usage percentage

#endif // RESOURCE_MONITOR_H
