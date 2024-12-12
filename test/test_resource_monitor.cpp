// test/test_resource_monitor.cpp

/**
 * @file test_resource_monitor.cpp
 * @description Implements unit tests for the Resource Monitor module using Google Test.
 *
 * This test suite verifies the functionality of the Resource Monitor module, specifically
 * focusing on the retrieval and calculation of CPU and process times. The tests ensure
 * that the functions `getTotalCpuTime` and `getProcessTotalTime` return valid and
 * meaningful values. These tests help in validating the accuracy and reliability of the
 * resource monitoring mechanisms within the application.
 */

#include "resource_monitor.h"
#include "process_info.h"
#include <gtest/gtest.h>

/**
 * @brief Tests that the `getTotalCpuTime` function returns a value greater than zero.
 *
 * This test ensures that the `getTotalCpuTime` function correctly retrieves the total CPU
 * time from the system's `/proc/stat` file. A valid total CPU time should always be greater
 * than zero, indicating that the function is successfully reading and summing CPU time fields.
 */
TEST(ResourceMonitorTest, GetTotalCpuTime) {
    long totalCpuTime = getTotalCpuTime();
    EXPECT_GT(totalCpuTime, 0) << "Total CPU time should be greater than zero";
}

/**
 * @brief Tests that the `getProcessTotalTime` function returns a non-negative value for a valid PID.
 *
 * This test retrieves a list of active processes using the `getActiveProcesses` function. If the
 * list is not empty, it selects the first process and verifies that the `getProcessTotalTime`
 * function returns a non-negative CPU time value. A non-negative value indicates that the function
 * is correctly parsing and summing CPU time fields from the process's `/proc/[pid]/stat` file.
 *
 * If no active processes are available, the test is skipped to prevent false failures.
 */
TEST(ResourceMonitorTest, GetProcessTotalTime) {
    std::vector<Process> processes = getActiveProcesses();
    if (!processes.empty()) {
        int pid = processes.front().pid;
        long processTime = getProcessTotalTime(pid);
        EXPECT_GE(processTime, 0) << "Process total time should be non-negative";
    } else {
        GTEST_SKIP() << "No processes available to test";
    }
}
