// test/test_process_info.cpp

/**
 * @file test_process_info.cpp
 * @description Implements unit tests for the Process Info module using Google Test.
 *
 * This test suite verifies the functionality and reliability of the Process Info module,
 * specifically focusing on ensuring that the retrieval of active processes is accurate
 * and consistent. The tests check for the absence of duplicate PIDs and validate the
 * integrity of the data associated with each process, such as PID validity, user assignment,
 * command association, and memory usage metrics.
 */

#include "process_info.h"
#include <gtest/gtest.h>
#include <unordered_set>

/**
 * @brief Tests that there are no duplicate Process IDs (PIDs) in the list of active processes.
 *
 * This test retrieves the list of active processes using the `getActiveProcesses` function
 * and verifies that each PID is unique. It does so by inserting each PID into an unordered
 * set and checking that the insertion was successful, which would indicate that the PID was not
 * already present in the set. If a duplicate PID is found, the test will fail and report the
 * conflicting PID.
 */
TEST(ProcessInfoTest, NoDuplicatePIDs) {
    // Retrieve the list of active processes
    std::vector<Process> processes = getActiveProcesses();
    std::unordered_set<int> pidSet;

    // Iterate through each process to ensure PIDs are unique
    for (const auto& process : processes) {
        auto result = pidSet.insert(process.pid);
        EXPECT_TRUE(result.second) << "Duplicate PID found: " << process.pid;
    }
}

/**
 * @brief Tests the consistency and validity of process data retrieved by the Process Info module.
 *
 * This test ensures that each process in the list of active processes has valid and consistent data.
 * Specifically, it checks that:
 * - The PID is a positive integer.
 * - The user associated with the process is not an empty string.
 * - The command associated with the process is not an empty string.
 * - The memory usage reported for the process is a non-negative value.
 *
 * If any of these conditions are not met for a process, the test will fail and provide a descriptive
 * message indicating the nature of the inconsistency.
 */
TEST(ProcessInfoTest, DataConsistency) {
    // Retrieve the list of active processes
    std::vector<Process> processes = getActiveProcesses();

    // Iterate through each process to validate data consistency
    for (const auto& process : processes) {
        // Verify that the PID is a positive integer
        EXPECT_GT(process.pid, 0) << "Invalid PID: " << process.pid;

        // Verify that the user associated with the process is not empty
        EXPECT_FALSE(process.user.empty()) << "User is empty for PID: " << process.pid;

        // Verify that the command associated with the process is not empty
        EXPECT_FALSE(process.command.empty()) << "Command is empty for PID: " << process.pid;

        // Verify that the memory usage is non-negative
        EXPECT_GE(process.memoryUsage, 0.0) << "Negative memory usage for PID: " << process.pid;
    }
}
