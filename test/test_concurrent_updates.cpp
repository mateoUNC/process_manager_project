#include "resource_monitor.h"
#include "process_info.h"
#include "globals.h"
#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
TEST(ConcurrencyTest, MonitorConsistency) {
    // Shared data structures
    std::unordered_map<int, Process> sharedProcesses;
    std::mutex sharedMutex;

    // Simulated worker threads
    auto updateCpuUsage = [&]() {
        for (int i = 0; i < 10; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            std::lock_guard<std::mutex> lock(sharedMutex);

            // Simulate CPU updates
            int pid = i + 1;
            sharedProcesses[pid].pid = pid;
            sharedProcesses[pid].cpuUsage += 10.0;
        }
    };

    auto updateMemoryUsage = [&]() {
        for (int i = 0; i < 10; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            std::lock_guard<std::mutex> lock(sharedMutex);

            // Simulate memory updates
            int pid = i + 1;
            sharedProcesses[pid].pid = pid;
            sharedProcesses[pid].memoryUsage += 100.0;
        }
    };

    // Spawn threads
    std::thread cpuThread(updateCpuUsage);
    std::thread memoryThread(updateMemoryUsage);

    // Join threads
    cpuThread.join();
    memoryThread.join();

    // Verify consistency
    for (const auto& [pid, process] : sharedProcesses) {
        EXPECT_EQ(process.pid, pid);
        EXPECT_DOUBLE_EQ(process.cpuUsage, 10.0) << "CPU usage should be consistent";
        EXPECT_DOUBLE_EQ(process.memoryUsage, 100.0) << "Memory usage should be consistent";
    }
}


TEST(ThreadResultManagementTest, CPUAndMemorySynchronization) {
    const int numThreads = 5;
    const int numProcessesPerThread = 50;

    // Simulate multiple threads updating the global processes map
    std::vector<std::thread> threads;

    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([t, numProcessesPerThread]() {
            for (int i = 0; i < numProcessesPerThread; ++i) {
                int pid = t * numProcessesPerThread + i;

                // Simulate a process
                Process dummyProcess;
                dummyProcess.pid = pid;
                dummyProcess.user = "user" + std::to_string(pid % 5);
                dummyProcess.cpuUsage = pid % 100;       // Simulate CPU usage
                dummyProcess.memoryUsage = pid * 1.5;   // Simulate memory usage

                // Lock to safely modify shared data
                {
                    std::lock_guard<std::mutex> lock(processMutex);
                    processes[pid] = dummyProcess; // Add or update process
                }

                // Simulate a small delay to increase contention
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        });
    }

    // Join all threads
    for (auto& thread : threads) {
        thread.join();
    }

    // Validate results
    std::unordered_set<int> seenPIDs;
    {
        std::lock_guard<std::mutex> lock(processMutex);

        // Check for duplicate PIDs and validate process data
        for (const auto& [pid, process] : processes) {
            ASSERT_EQ(seenPIDs.count(pid), 0) << "Duplicate PID found: " << pid;
            seenPIDs.insert(pid);

            // Validate process data
            ASSERT_GE(process.cpuUsage, 0) << "CPU usage should be non-negative";
            ASSERT_GE(process.memoryUsage, 0) << "Memory usage should be non-negative";
        }
    }

    // Verify the total number of processes added
    ASSERT_EQ(processes.size(), numThreads * numProcessesPerThread)
        << "Unexpected number of processes in the map";
}
