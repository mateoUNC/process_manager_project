//process_manager.cpp

#include "process_manager.h"
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <sstream>
#include <unistd.h>

// Function to fetch data for a single process
void fetchProcessData(int pid, std::vector<Process>& processes) {
    std::ifstream commFile("/proc/" + std::to_string(pid) + "/comm");
    std::ifstream statFile("/proc/" + std::to_string(pid) + "/stat");
    std::ifstream statusFile("/proc/" + std::to_string(pid) + "/status");

    if (commFile.is_open() && statFile.is_open() && statusFile.is_open()) {
        std::string processName;
        std::getline(commFile, processName);
        
        // Extracting CPU usage from /proc/[pid]/stat
        std::string statData;
        std::getline(statFile, statData);
        std::istringstream statStream(statData);
        
        long utime, stime;
        statStream >> std::skipws;  // Skip whitespace characters
        for (int i = 0; i < 13; ++i) statStream.ignore(100, ' ');  // Skip irrelevant fields
        statStream >> utime >> stime;  // User and system CPU time
        
        double cpuUsage = (utime + stime) / static_cast<double>(sysconf(_SC_CLK_TCK));  // Convert jiffies to seconds

        // Extracting memory usage from /proc/[pid]/status
        std::string line;
        long vmRSS = 0;  // VmRSS (Resident Set Size)
        while (std::getline(statusFile, line)) {
            if (line.find("VmRSS") == 0) {
                std::istringstream(line.substr(6)) >> vmRSS;  // Extract the VmRSS value (in KB)
                break;
            }
        }

        double memoryUsage = vmRSS / 1024.0;  // Convert to MB

        // Create the Process struct and add it to a local vector
        Process proc = {pid, processName, cpuUsage, memoryUsage};
        processes.push_back(proc);
    }
}

// Function to process data for a batch of processes
void batchProcessData(const std::vector<int>& pids, std::vector<Process>& processes) {
    std::vector<Process> localProcesses;
    for (int pid : pids) {
        fetchProcessData(pid, localProcesses);
    }
    // Merge the local data into the shared processes vector
    processes.insert(processes.end(), localProcesses.begin(), localProcesses.end());
}

// Main function to list processes using multithreading and thread pool (batch processing)
void listProcesses() {
    std::vector<Process> processes;

    // Open the /proc directory to get the list of PIDs
    DIR* dir = opendir("/proc");
    if (dir == nullptr) {
        std::cerr << "Failed to open /proc directory!" << std::endl;
        return;
    }

    struct dirent* entry;
    std::vector<int> pids;

    // Iterate through the /proc directory and collect PIDs
    while ((entry = readdir(dir)) != nullptr) {
        if (isdigit(entry->d_name[0])) {
            int pid = std::stoi(entry->d_name);
            pids.push_back(pid);
        }
    }

    closedir(dir);

    // Now we process the PIDs in batches using multiple threads
    size_t batchSize = 100; // Number of processes each thread will handle
    std::vector<std::future<void>> futures;

    // Split PIDs into batches and process each batch in a separate thread
    for (size_t i = 0; i < pids.size(); i += batchSize) {
        // Create a batch of PIDs to process
        std::vector<int> batch(pids.begin() + i, pids.begin() + std::min(i + batchSize, pids.size()));

        // Launch a thread to process the batch using a lambda
        futures.push_back(std::async(std::launch::async, [batch, &processes]() {
            batchProcessData(batch, processes);
        }));
    }

    // Wait for all threads to finish
    for (auto& fut : futures) {
        fut.get();
    }

    // Now print out the processes with their CPU and memory usage
    for (const auto& process : processes) {
        std::cout << process.pid << "  "
                  << process.name << "  "
                  << "CPU: " << process.cpuUsage << "s  "
                  << "Memory: " << process.memoryUsage << "MB\n";
    }
}

// Function to return active processes
std::vector<Process> getActiveProcesses() {
    std::vector<Process> processes;
    std::vector<int> pids;

    // Open /proc to get the list of PIDs
    DIR* dir = opendir("/proc");
    if (dir == nullptr) {
        std::cerr << "Failed to open /proc directory!" << std::endl;
        return processes;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (isdigit(entry->d_name[0])) {
            int pid = std::stoi(entry->d_name);
            pids.push_back(pid);
        }
    }

    closedir(dir);

    size_t batchSize = 100;  // Number of processes to process per thread
    std::vector<std::future<void>> futures;

    // Process the PIDs in batches
    for (size_t i = 0; i < pids.size(); i += batchSize) {
        std::vector<int> batch(pids.begin() + i, pids.begin() + std::min(i + batchSize, pids.size()));
        futures.push_back(std::async(std::launch::async, [batch, &processes]() {
            batchProcessData(batch, processes);
        }));
    }

    // Wait for all threads to complete
    for (auto& fut : futures) {
        fut.get();
    }

    return processes;
}
