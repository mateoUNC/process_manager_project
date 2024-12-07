// test/test_concurrent_updates.cpp
#include <thread>
#include <future>
#include <unordered_map>
#include <vector>
#include <mutex>
#include "process_info.h"
#include "resource_monitor.h"
#include <gtest/gtest.h>

// extern std::mutex processMutex; // Ensure this is accessible or make it accessible for testing

// TEST(ResourceMonitorTest, ConcurrentMemoryUsageUpdates) {
//     std::unordered_map<int, Process> processes;
//     std::vector<Process> activeProcesses = getActiveProcesses();

//     // Run multiple threads to update memory usage concurrently
//     std::vector<std::thread> threads;
//     const int numThreads = 4; // For example
//     for (int i = 0; i < numThreads; ++i) {
//         threads.emplace_back(updateMemoryUsage, std::ref(processes), std::cref(activeProcesses));
//     }

//     // Wait for all threads to finish
//     for (auto& t : threads) {
//         t.join();
//     }

//     // Verify that the processes map is consistent
//     EXPECT_FALSE(processes.empty()) << "Processes map should not be empty";

//     // Additional consistency checks
//     for (const auto& entry : processes) {
//         const Process& process = entry.second;
//         EXPECT_GE(process.memoryUsage, 0.0) << "Memory usage should be non-negative";
//         // Add more checks as needed
//     }
// }
