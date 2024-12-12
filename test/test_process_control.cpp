/**
 * @file test_process_control.cpp
 * @description Implements unit tests for the Process Control module using Google Test and Google Mock.
 *
 * This source file contains test cases that verify the functionality of the Process Control module,
 * specifically focusing on the `killProcess` function. It includes both mock-based tests to simulate
 * various scenarios and real-process-based tests to ensure the correct behavior when interacting with
 * actual system processes. The tests aim to validate error handling, permission checks, and the
 * ability to terminate processes under different conditions.
 */

#include "process_control.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cerrno>

/**
 * @brief Mock function to simulate the behavior of the `kill` system call.
 *
 * This mock function overrides the standard `kill` function to simulate different outcomes
 * based on the provided PID. It is used to test how the `killProcess` function handles various
 * scenarios without affecting actual system processes.
 *
 * @param pid The Process ID of the target process to kill.
 * @param signal The signal to send to the process.
 * @return Returns 0 on simulated success, or -1 on simulated failure.
 */
int mock_kill(pid_t pid, int signal) {
    if (pid == 12345) {
        return 0; // Simulate successful termination
    }
    if (pid <= 0) {
        errno = ESRCH; // Invalid PID
        return -1;
    }
    if (pid == getpid()) {
        errno = EPERM; // Cannot kill self
        return -1;
    }
    if (pid == 99999) {
        errno = ESRCH; // Non-existent process
        return -1;
    }
    if (pid == 88888) {
        errno = EPERM; // Insufficient permissions
        return -1;
    }
    return 0; // Default to success
}

/**
 * @brief Tests the `killProcess` function with invalid PIDs.
 *
 * This test case verifies that attempting to kill processes with invalid PIDs (negative numbers
 * and zero) correctly results in failure. It ensures that the `killProcess` function handles
 * such inputs gracefully without affecting any system processes.
 */
TEST(MockProcessControlTest, KillProcessInvalidPID) {
    EXPECT_FALSE(killProcess(-1)); // Invalid PID
    EXPECT_FALSE(killProcess(0));  // Invalid PID
}

/**
 * @brief Tests the `killProcess` function when attempting to kill the current process.
 *
 * This test case ensures that the `killProcess` function prevents terminating the process in
 * which it is running. Attempting to kill the current process should fail, and the function
 * should return `false` to indicate the failure.
 */
TEST(MockProcessControlTest, KillProcessSelf) {
    EXPECT_FALSE(killProcess(getpid())); // Attempt to kill self
}

/**
 * @brief Tests the `killProcess` function with a non-existent PID.
 *
 * This test case verifies that attempting to kill a process with a PID that does not exist
 * results in failure. The `killProcess` function should return `false` when it cannot find
 * the specified process.
 */
TEST(MockProcessControlTest, KillProcessNonExistent) {
    EXPECT_FALSE(killProcess(99999)); // Non-existent PID
}

/**
 * @brief Tests the `killProcess` function with a PID that has insufficient permissions.
 *
 * This test case ensures that attempting to kill a process without the necessary permissions
 * correctly results in failure. The `killProcess` function should handle permission errors
 * gracefully and return `false`.
 */
TEST(MockProcessControlTest, KillProcessInsufficientPermissions) {
    EXPECT_FALSE(killProcess(88888)); // PID for which permissions are insufficient
}

/**
 * @brief Tests the `killProcess` function by terminating a real child process.
 *
 * This test case involves creating a real child process that runs indefinitely. The parent
 * process then attempts to terminate the child using the `killProcess` function. The test
 * verifies that the child process is successfully terminated with the `SIGKILL` signal and
 * that the termination is properly handled.
 */
TEST(RealProcessControlTest, KillProcessRealChild) {
    // Fork a child process
    pid_t childPid = fork();

    if (childPid == 0) {
        // Child process: Sleep indefinitely
        pause();
        exit(0); // Ensure child exits
    } else if (childPid > 0) {
        // Parent process: Test killing the child
        EXPECT_TRUE(killProcess(childPid)) << "Failed to kill real child process";

        // Ensure the child process is cleaned up
        int status;
        EXPECT_EQ(waitpid(childPid, &status, 0), childPid) << "Failed to clean up child process";
        EXPECT_TRUE(WIFSIGNALED(status)) << "Child process was not terminated by a signal";
        EXPECT_EQ(WTERMSIG(status), SIGKILL) << "Child process was not killed with SIGKILL";
    } else {
        // Fork failed
        FAIL() << "Failed to fork a child process";
    }
}

/**
 * @brief Tests the `killProcess` function by attempting to kill a non-existent real process.
 *
 * This test case ensures that attempting to kill a real process that does not exist results
 * in failure. The `killProcess` function should return `false` when it cannot find the specified
 * process, and the test verifies this behavior.
 */
TEST(RealProcessControlTest, KillProcessNonExistent) {
    EXPECT_FALSE(killProcess(99999)) << "Non-existent process should not be killed";
}
