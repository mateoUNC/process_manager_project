/**
 * @file globals.cpp
 * @brief Defines global variables used throughout the Process Manager application.
 *
 * This source file implements the global variables, mutexes, condition variables, and data structures
 * that are shared across different modules for process monitoring, synchronization, and state management.
 * These globals facilitate inter-thread communication and maintain the application's state.
 */

#include "globals.h"

/**
 * @brief Atomic flag indicating whether monitoring is currently active.
 *
 * Initialized to `false`. When set to `true`, monitoring threads collect and update process data.
 */
std::atomic<bool> monitoringActive(false);

// Mutex to synchronize access to standard output (std::cout) to prevent race conditions.
std::mutex coutMutex;

// Mutex to protect access to the shared processes map, ensuring thread-safe operations.
std::mutex processMutex;

// Condition variable used to coordinate thread activities, such as pausing and resuming monitoring.
std::condition_variable cv;

// Mutex associated with the condition variable for synchronizing access.
std::mutex cvMutex;

// String representing the current sorting criterion for displaying processes.
// Defaults to sorting by CPU usage.
std::string sortingCriterion = "cpu";

// Unordered map storing information about monitored processes, indexed by their Process ID (PID).
std::unordered_map<int, Process> processes;

/**
 * @brief Atomic flag indicating whether monitoring is currently Paused.
 *
 * Initialized to `false`. When set to `true`, monitoring threads collect and update process data.
 */
std::atomic<bool> monitoringPaused(false);

// Pair representing the current filter criterion:
// - First element: Type of filter ("user", "cpu", "memory", or "none").
// - Second element: Value associated with the filter.
// Defaults to no filter.
std::pair<std::string, std::string> filterCriterion = {"none", ""};

// Cache mapping PIDs to usernames to reduce redundant lookups and improve performance.
std::unordered_map<int, std::string> pidToUserCache;

// Cache mapping PIDs to command names to reduce redundant lookups and improve performance.
std::unordered_map<int, std::string> pidToCommandCache;

/**
 * @brief Frecuency update.
 *
 * Default in 5 seconds.
 */
std::atomic<int> updateFrequency(5);
