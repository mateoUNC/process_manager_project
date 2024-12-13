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

/**
 * @brief Mutex to synchronize access to standard output (std::cout).
 *
 * Prevents race conditions and ensures thread-safe console output.
 */
std::mutex coutMutex;

/**
 * @brief Mutex to protect access to the shared processes map.
 *
 * Ensures thread-safe operations when reading or updating the `processes` map.
 */
std::mutex processMutex;

/**
 * @brief Condition variable used to coordinate thread activities.
 *
 * Pauses or resumes monitoring threads based on specific conditions.
 */
std::condition_variable cv;

/**
 * @brief Mutex associated with the condition variable.
 *
 * Provides thread-safe access to the condition variable.
 */
std::mutex cvMutex;

/**
 * @brief Current sorting criterion for process display.
 *
 * Defaults to `"cpu"`. Determines the order in which processes are displayed (e.g., by CPU or memory usage).
 */
std::string sortingCriterion = "cpu";

/**
 * @brief Map storing information about monitored processes.
 *
 * Indexed by process ID (PID). Allows efficient access and updates of process information.
 */
std::unordered_map<int, Process> processes;

/**
 * @brief Atomic flag indicating whether monitoring is currently paused.
 *
 * Initialized to `false`. When set to `true`, monitoring threads stop updating data and wait.
 */
std::atomic<bool> monitoringPaused(false);

/**
 * @brief Current filter criterion for displaying processes.
 *
 * Defaults to `{"none", ""}`. Consists of:
 * - Filter type (e.g., `"user"`, `"cpu"`, `"memory"`).
 * - Filter value (e.g., `"root"`, `50%`, `"100 MB"`).
 */
std::pair<std::string, std::string> filterCriterion = {"none", ""};

/**
 * @brief Cache mapping PIDs to usernames.
 *
 * Reduces redundant lookups by storing associations between process IDs and usernames.
 */
std::unordered_map<int, std::string> pidToUserCache;

/**
 * @brief Cache mapping PIDs to command names.
 *
 * Reduces redundant lookups by storing associations between process IDs and their commands.
 */
std::unordered_map<int, std::string> pidToCommandCache;

/**
 * @brief Frequency (in seconds) for updating resource monitoring data.
 *
 * Initialized to `5`. Determines how often monitoring threads update CPU and memory usage information.
 */
std::atomic<int> updateFrequency(5);
