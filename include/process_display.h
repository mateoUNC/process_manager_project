/**
 * @file process_display.h
 * @brief Declares functions for displaying process information to the user.
 *
 * This header file provides function declarations for printing process details to the console,
 * applying filters and sorting criteria to enhance readability and usability.
 */

#ifndef PROCESS_DISPLAY_H
#define PROCESS_DISPLAY_H

#include "process_info.h"
#include <vector>

/**
 * @brief Displays a list of processes in a formatted table.
 *
 * Prints the details of each process, including PID, user, CPU usage, memory usage, and command,
 * to the console. Applies current sorting and filtering criteria to determine the order and inclusion
 * of processes in the display.
 *
 * @param processes A vector of Process structs containing information about active processes.
 */
void printProcesses(const std::vector<Process>& processes);

#endif // PROCESS_DISPLAY_H
