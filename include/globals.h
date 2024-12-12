/**
 * @file globals.h
 * @brief Declares global variables and synchronization primitives used across the Process Manager application.
 *
 * This header file defines global atomic flags, mutexes, condition variables, and data structures
 * that are shared among various modules for process monitoring, synchronization, and state management.
 */

#ifndef GLOBALS_H
#define GLOBALS_H

#include "process_info.h" // Include the Process struct and related functions
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <unordered_map>

/**
 * @brief Atomic flag indicating whether monitoring is active.
 *
 * When `true`, the monitoring threads are actively collecting and updating process data.
 * When `false`, monitoring is stopped or paused.
 */
extern std::atomic<bool> monitoringActive;

/**
 * @brief Mutex to synchronize access to standard output (std::cout).
 *
 * Ensures thread-safe console output to prevent race conditions and garbled text.
 */
extern std::mutex coutMutex;

/**
 * @brief Mutex to synchronize access to the processes map.
 *
 * Protects shared access to the `processes` map, ensuring thread-safe operations.
 */
extern std::mutex processMutex;

/**
 * @brief Condition variable used for thread synchronization.
 *
 * Used in conjunction with `cvMutex` to pause or resume monitoring threads.
 */
extern std::condition_variable cv;

/**
 * @brief Mutex associated with the condition variable for synchronization.
 */
extern std::mutex cvMutex;

/**
 * @brief String indicating the current sorting criterion ("cpu" or "memory").
 *
 * Determines the order in which monitored processes are displayed.
 */
extern std::string sortingCriterion;

/**
 * @brief Map storing process information indexed by PID.
 *
 * This unordered map maintains the current state of monitored processes, allowing
 * efficient access and updates based on process IDs.
 */
extern std::unordered_map<int, Process> processes;

/**
 * @brief Atomic flag indicating whether monitoring is paused.
 *
 * When `true`, monitoring threads should stop updating data and wait.
 */
extern std::atomic<bool> monitoringPaused;

/**
 * @brief Pair representing the current filter criterion.
 *
 * The first element specifies the filter type (e.g., "user", "cpu", "memory"),
 * and the second element specifies the filter value.
 */
extern std::pair<std::string, std::string> filterCriterion;

/**
 * @brief Cache mapping PIDs to usernames.
 *
 * This unordered map caches the association between process IDs and their corresponding
 * usernames to minimize repeated lookups.
 */
extern std::unordered_map<int, std::string> pidToUserCache;

/**
 * @brief Cache mapping PIDs to command names.
 *
 * This unordered map caches the association between process IDs and their corresponding
 * command names to minimize repeated lookups.
 */
extern std::unordered_map<int, std::string> pidToCommandCache;

/**
 * @brief Atomic integer representing the update frequency in seconds.
 *
 * Determines how often the monitoring threads update CPU and memory usage information.
 * Defaults to updating every 5 seconds.
 */
extern std::atomic<int> updateFrequency;

#endif // GLOBALS_H
