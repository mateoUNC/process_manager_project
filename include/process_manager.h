#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <future>

// Process structure as you defined earlier
struct Process {
    int pid;
    std::string name;
    double cpuUsage;
    double memoryUsage;
};

std::vector<Process> getActiveProcesses();
void fetchProcessData(int pid, std::vector<Process>& processes);
void listProcesses();  // Entry function to get processes using multithreading
void batchProcessData(const std::vector<int>& pids, std::vector<Process>& processes);

#endif // PROCESS_MANAGER_H
