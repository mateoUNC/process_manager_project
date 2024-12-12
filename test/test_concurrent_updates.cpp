/**
 * @file test_resource_monitor.cpp
 * @description Implements unit tests for the Resource Monitor module using Google Test.
 *
 * This source file contains test cases that verify the correctness and thread safety
 * of the Resource Monitor module. It includes tests for concurrency to ensure that
 * shared data structures remain consistent when accessed by multiple threads simultaneously.
 * The tests simulate updates to CPU and memory usage and validate that the monitoring
 * mechanisms function as expected under concurrent conditions.
 */

#include "resource_monitor.h"
#include "process_info.h"
#include "globals.h"
#include "logger.h" // Include the Logger header
#include "process_display.h"
#include "process_info.h" // For getActiveProcesses()
#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <chrono>

// Test case to verify the consistency of monitoring in a concurrent environment
TEST(ConcurrencyTest, MonitorConsistency) {
    // Shared data structures for the test
    std::unordered_map<int, Process> sharedProcesses;
    std::mutex sharedMutex;

    // Lambda function to simulate CPU usage updates
    auto updateCpuUsage = [&]() {
        for (int i = 0; i < 10; ++i) {
            // Simulate processing delay
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            // Lock the mutex to ensure thread-safe access to sharedProcesses
            std::lock_guard<std::mutex> lock(sharedMutex);

            // Simulate CPU usage updates for a process
            int pid = i + 1;
            sharedProcesses[pid].pid = pid;
            sharedProcesses[pid].cpuUsage += 10.0;
        }
    };

    // Lambda function to simulate memory usage updates
    auto updateMemoryUsage = [&]() {
        for (int i = 0; i < 10; ++i) {
            // Simulate processing delay
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            // Lock the mutex to ensure thread-safe access to sharedProcesses
            std::lock_guard<std::mutex> lock(sharedMutex);

            // Simulate memory usage updates for a process
            int pid = i + 1;
            sharedProcesses[pid].pid = pid;
            sharedProcesses[pid].memoryUsage += 100.0;
        }
    };

    // Spawn threads to perform CPU and memory updates concurrently
    std::thread cpuThread(updateCpuUsage);
    std::thread memoryThread(updateMemoryUsage);

    // Wait for both threads to complete their execution
    cpuThread.join();
    memoryThread.join();

    // Verify that the updates were performed consistently
    for (const auto& [pid, process] : sharedProcesses) {
        // Check that the PID matches the expected value
        EXPECT_EQ(process.pid, pid);

        // Verify that CPU usage was incremented correctly
        EXPECT_DOUBLE_EQ(process.cpuUsage, 10.0) << "CPU usage should be consistent";

        // Verify that memory usage was incremented correctly
        EXPECT_DOUBLE_EQ(process.memoryUsage, 100.0) << "Memory usage should be consistent";
    }
}

// Test case to verify synchronization between CPU and Memory updates across multiple threads
TEST(ThreadResultManagementTest, CPUAndMemorySynchronization) {
    const int numThreads = 5;               // Number of worker threads to spawn
    const int numProcessesPerThread = 50;   // Number of processes each thread will handle

    // Vector to hold the spawned threads
    std::vector<std::thread> threads;

    // Launch multiple threads to simulate concurrent updates to the global processes map
    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([t, numProcessesPerThread]() {
            for (int i = 0; i < numProcessesPerThread; ++i) {
                int pid = t * numProcessesPerThread + i;

                // Create a dummy Process object with simulated data
                Process dummyProcess;
                dummyProcess.pid = pid;
                dummyProcess.user = "user" + std::to_string(pid % 5);
                dummyProcess.cpuUsage = pid % 100;       // Simulate CPU usage percentage
                dummyProcess.memoryUsage = pid * 1.5;    // Simulate memory usage in MB

                // Lock the mutex to safely modify the shared processes map
                {
                    std::lock_guard<std::mutex> lock(processMutex);
                    processes[pid] = dummyProcess; // Add or update the process in the global map
                }

                // Introduce a small delay to increase thread contention
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        });
    }

    // Wait for all threads to complete their execution
    for (auto& thread : threads) {
        thread.join();
    }

    // Validate the results to ensure data consistency and correctness
    std::unordered_set<int> seenPIDs; // Set to track unique PIDs
    {
        // Lock the mutex to safely access the shared processes map
        std::lock_guard<std::mutex> lock(processMutex);

        // Iterate through all processes in the global map
        for (const auto& [pid, process] : processes) {
            // Check for duplicate PIDs
            ASSERT_EQ(seenPIDs.count(pid), 0) << "Duplicate PID found: " << pid;
            seenPIDs.insert(pid);

            // Validate that CPU usage is non-negative
            ASSERT_GE(process.cpuUsage, 0) << "CPU usage should be non-negative";

            // Validate that memory usage is non-negative
            ASSERT_GE(process.memoryUsage, 0) << "Memory usage should be non-negative";
        }
    }

    // Verify that the total number of processes matches the expected count
    ASSERT_EQ(processes.size(), numThreads * numProcessesPerThread)
        << "Unexpected number of processes in the map";
}
