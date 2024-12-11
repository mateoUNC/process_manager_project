#include "process_control.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cerrno>

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

TEST(MockProcessControlTest, KillProcessInvalidPID) {
    EXPECT_FALSE(killProcess(-1)); // Invalid PID
    EXPECT_FALSE(killProcess(0));  // Invalid PID
}

TEST(MockProcessControlTest, KillProcessSelf) {
    EXPECT_FALSE(killProcess(getpid())); // Attempt to kill self
}

TEST(MockProcessControlTest, KillProcessNonExistent) {
    EXPECT_FALSE(killProcess(99999)); // Non-existent PID
}

TEST(MockProcessControlTest, KillProcessInsufficientPermissions) {
    EXPECT_FALSE(killProcess(88888)); // PID for which permissions are insufficient
}

// Real-process-based tests
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

TEST(RealProcessControlTest, KillProcessNonExistent) {
    EXPECT_FALSE(killProcess(99999)) << "Non-existent process should not be killed";
}
