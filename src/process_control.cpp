/**
 * @file process_control.cpp
 * @brief Implements functions for controlling and terminating processes.
 *
 * This source file contains the implementation of functions that allow the user to terminate
 * individual processes by PID, as well as groups of processes based on CPU usage thresholds
 * or user ownership. It ensures safe termination by handling various error scenarios such as
 * invalid PIDs, insufficient permissions, and attempts to kill critical processes like the
 * current process. Thread safety is maintained through the use of mutexes when accessing
 * the shared processes map.
 */

#include "process_control.h"
#include "globals.h"
#include <errno.h>  // For errno
#include <iostream> // For std::cerr and std::cout
#include <signal.h> // For kill()
#include <unistd.h> // For getpid()

/**
 * @brief Attempts to terminate a process with the specified PID.
 *
 * This function sends a SIGKILL signal to the process identified by the given PID.
 * It performs several checks to ensure that the PID is valid and that the process
 * can be safely terminated. It handles common errors such as invalid PIDs,
 * insufficient permissions, and attempts to kill the current process.
 *
 * @param pid The Process ID of the target process to kill.
 * @return true if the process was successfully terminated, false otherwise.
 */
bool killProcess(int pid)
{
    // Validate PID
    if (pid <= 0)
    {
        std::cerr << "Error: Invalid PID (" << pid << ").\n";
        return false;
    }

    // Prevent killing the current process
    if (pid == getpid())
    {
        std::cerr << "Error: Cannot kill the current process (PID=" << pid << ").\n";
        return false;
    }

    // Attempt to send SIGKILL to the process
    if (kill(pid, SIGKILL) == 0)
    {
        std::cout << "Process " << pid << " terminated successfully.\n";
        return true;
    }
    else
    {
        // Handle specific errors based on errno
        switch (errno)
        {
        case ESRCH:
            std::cerr << "Error: Process " << pid << " does not exist.\n";
            break;
        case EPERM:
            std::cerr << "Error: Insufficient permissions to kill process " << pid << ".\n";
            break;
        default:
            perror("Error killing process");
            break;
        }
        return false;
    }
}

/**
 * @brief Terminates all processes exceeding a specified CPU usage threshold.
 *
 * This function iterates through the monitored processes and sends a SIGKILL signal
 * to any process whose CPU usage surpasses the provided threshold. It provides a
 * summary of successful and failed termination attempts. Thread safety is ensured
 * by locking the processes map during iteration.
 *
 * @param threshold The CPU usage percentage threshold. Processes exceeding this value will be killed.
 * @return true if at least one process was successfully terminated, false otherwise.
 */
bool killProcessesByCpu(double threshold)
{
    bool anyKilled = false;
    int successCount = 0;
    int failureCount = 0;

    {
        // Lock the processes map to ensure thread-safe access
        std::lock_guard<std::mutex> lock(processMutex);
        for (const auto& [pid, process] : processes)
        {
            if (process.cpuUsage > threshold)
            {
                // Attempt to kill the process
                if (kill(pid, SIGKILL) == 0)
                {
                    std::cout << "Killed process " << pid << " (CPU: " << process.cpuUsage << "%)\n";
                    anyKilled = true;
                    successCount++;
                }
                else
                {
                    std::cerr << "Failed to kill process " << pid << ": ";
                    switch (errno)
                    {
                    case ESRCH:
                        std::cerr << "Process does not exist.\n";
                        break;
                    case EPERM:
                        std::cerr << "Insufficient permissions.\n";
                        break;
                    default:
                        perror("Error");
                        break;
                    }
                    failureCount++;
                }
            }
        }
    }

    // Provide a summary of the termination attempts
    if (!anyKilled)
    {
        std::cout << "No processes found exceeding the CPU usage threshold.\n";
    }
    else
    {
        std::cout << "Summary: " << successCount << " processes killed, " << failureCount << " failed.\n";
    }

    return anyKilled;
}

/**
 * @brief Terminates all processes owned by a specified user.
 *
 * This function iterates through the monitored processes and sends a SIGKILL signal
 * to any process owned by the specified username. It provides a summary of successful
 * and failed termination attempts. Thread safety is ensured by locking the processes
 * map during iteration.
 *
 * @param username The username whose processes should be terminated.
 * @return true if at least one process was successfully terminated, false otherwise.
 */
bool killProcessesByUser(const std::string& username)
{
    bool anyKilled = false;
    int successCount = 0;
    int failureCount = 0;

    {
        // Lock the processes map to ensure thread-safe access
        std::lock_guard<std::mutex> lock(processMutex);
        for (const auto& [pid, process] : processes)
        {
            if (process.user == username)
            {
                // Attempt to kill the process
                if (kill(pid, SIGKILL) == 0)
                {
                    std::cout << "Killed process " << pid << " (User: " << username << ")\n";
                    anyKilled = true;
                    successCount++;
                }
                else
                {
                    std::cerr << "Failed to kill process " << pid << ": ";
                    switch (errno)
                    {
                    case ESRCH:
                        std::cerr << "Process does not exist.\n";
                        break;
                    case EPERM:
                        std::cerr << "Insufficient permissions.\n";
                        break;
                    default:
                        perror("Error");
                        break;
                    }
                    failureCount++;
                }
            }
        }
    }

    // Provide a summary of the termination attempts
    if (!anyKilled)
    {
        std::cout << "No processes found for user: " << username << "\n";
    }
    else
    {
        std::cout << "Summary: " << successCount << " processes killed, " << failureCount << " failed.\n";
    }

    return anyKilled;
}
