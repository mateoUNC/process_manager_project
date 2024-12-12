/**
 * @file process_control.h
 * @brief Declares functions for controlling and terminating processes.
 *
 * This header file provides function declarations for killing individual processes or groups of processes
 * based on CPU usage or user ownership. It includes error handling to ensure safe termination of processes.
 */

#ifndef PROCESS_CONTROL_H
#define PROCESS_CONTROL_H

#include <string>

/**
 * @brief Attempts to terminate a process with the specified PID.
 *
 * Sends a SIGKILL signal to the process identified by the given PID. Handles common errors such as
 * invalid PIDs, insufficient permissions, and attempts to kill the current process.
 *
 * @param pid The Process ID of the target process to kill.
 * @return `true` if the process was successfully terminated, `false` otherwise.
 */
bool killProcess(int pid);

/**
 * @brief Terminates all processes exceeding a specified CPU usage threshold.
 *
 * Iterates through the monitored processes and sends a SIGKILL signal to any process whose CPU usage
 * surpasses the provided threshold. Provides a summary of successful and failed termination attempts.
 *
 * @param threshold The CPU usage percentage threshold. Processes exceeding this value will be killed.
 * @return `true` if at least one process was successfully terminated, `false` otherwise.
 */
bool killProcessesByCpu(double threshold);

/**
 * @brief Terminates all processes owned by a specified user.
 *
 * Iterates through the monitored processes and sends a SIGKILL signal to any process owned by the
 * specified username. Provides a summary of successful and failed termination attempts.
 *
 * @param username The username whose processes should be terminated.
 * @return `true` if at least one process was successfully terminated, `false` otherwise.
 */
bool killProcessesByUser(const std::string& username);

#endif // PROCESS_CONTROL_H
