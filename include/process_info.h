/**
 * @file process_info.h
 * @brief Declares structures and functions for retrieving process information.
 *
 * This header file defines the Process struct and provides function declarations for obtaining
 * details about active processes, such as user ownership, command names, and memory usage.
 */

#ifndef PROCESS_INFO_H
#define PROCESS_INFO_H

#include <string>
#include <vector>

/**
 * @struct Process
 * @brief Represents information about a single process.
 *
 * The Process struct stores various attributes of a process, including its PID, owner, CPU and
 * memory usage, previous CPU time, and the command associated with the process.
 */
struct Process
{
    int pid;             /**< Process ID */
    std::string user;    /**< User owning the process */
    double cpuUsage;     /**< CPU usage percentage */
    double memoryUsage;  /**< Memory usage in MB */
    long prevTotalTime;  /**< Previous total CPU time of the process */
    std::string command; /**< Command associated with the process */
};

/**
 * @brief Retrieves a list of all active processes.
 *
 * Scans the system's `/proc` filesystem to gather information about currently running processes.
 *
 * @return A vector of Process structs containing details of active processes.
 */
std::vector<Process> getActiveProcesses();

/**
 * @brief Retrieves the username associated with a specific process.
 *
 * Reads the `/proc/[pid]/status` file to determine the UID of the process and converts it to a username.
 *
 * @param pid The Process ID of the target process.
 * @return The username owning the process, or "Unknown" if it cannot be determined.
 */
std::string getProcessUser(int pid);

/**
 * @brief Retrieves the command name associated with a specific process.
 *
 * Reads the `/proc/[pid]/comm` file to obtain the command name of the process.
 *
 * @param pid The Process ID of the target process.
 * @return The command name, or "Unknown" if it cannot be determined.
 */
std::string getProcessCommand(int pid);

/**
 * @brief Retrieves the memory usage of a specific process.
 *
 * Reads the `/proc/[pid]/status` file to determine the Resident Set Size (RSS) memory usage.
 *
 * @param pid The Process ID of the target process.
 * @return The memory usage in megabytes (MB), or 0.0 if it cannot be determined.
 */
double getProcessMemoryUsage(int pid);

#endif // PROCESS_INFO_H
