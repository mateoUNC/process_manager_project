// test/test_process_info.cpp
#include "process_info.h"
#include <gtest/gtest.h>
#include <unordered_set>

TEST(ProcessInfoTest, NoDuplicatePIDs) {
    std::vector<Process> processes = getActiveProcesses();
    std::unordered_set<int> pidSet;

    for (const auto& process : processes) {
        auto result = pidSet.insert(process.pid);
        EXPECT_TRUE(result.second) << "Duplicate PID found: " << process.pid;
    }
}

TEST(ProcessInfoTest, DataConsistency) {
    std::vector<Process> processes = getActiveProcesses();

    for (const auto& process : processes) {
        EXPECT_GT(process.pid, 0) << "Invalid PID";
        EXPECT_FALSE(process.user.empty()) << "User is empty for PID: " << process.pid;
        EXPECT_FALSE(process.command.empty()) << "Command is empty for PID: " << process.pid;
        EXPECT_GE(process.memoryUsage, 0.0) << "Negative memory usage for PID: " << process.pid;
    }
}
