// test/test_resource_monitor.cpp
#include "resource_monitor.h"
#include "process_info.h"
#include <gtest/gtest.h>

TEST(ResourceMonitorTest, GetTotalCpuTime) {
    long totalCpuTime = getTotalCpuTime();
    EXPECT_GT(totalCpuTime, 0) << "Total CPU time should be greater than zero";
}

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
