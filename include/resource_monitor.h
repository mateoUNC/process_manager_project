#ifndef RESOURCE_MONITOR_H
#define RESOURCE_MONITOR_H

#include <vector>
#include "process_info.h"

// Function declarations
void monitorProcesses();
long getTotalCpuTime();
long getProcessTotalTime(int pid);
double calculateCpuUsage(long processTimeDelta, long totalCpuTimeDelta, long numCores);

#endif // RESOURCE_MONITOR_H
