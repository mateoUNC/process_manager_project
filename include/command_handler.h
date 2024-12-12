/**
 * @file command_handler.h
 * @brief Handles user commands and manages the command loop for the Process Manager application.
 *
 * This header file declares functions related to command handling, including starting the command loop,
 * handling signals, and displaying help information to the user.
 */

#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <string>

/** @brief ANSI color code to reset text formatting. */
#define RESET "\033[0m"

/** @brief ANSI color code for cyan text. */
#define CYAN "\033[36m"

/** @brief ANSI color code for yellow text. */
#define YELLOW "\033[33m"

/** @brief ANSI color code for green text. */
#define GREEN "\033[32m"

/** @brief ANSI color code for red text. */
#define RED "\033[31m"

/** @brief ANSI escape code for bold text. */
#define BOLD "\033[1m"

/**
 * @brief Starts the command processing loop.
 *
 * Initiates the interactive command loop where users can input commands to control the Process Manager.
 */
void startCommandLoop();

/**
 * @brief Handles the SIGINT signal (Ctrl+C).
 *
 * This function is invoked when the user sends a SIGINT signal (typically by pressing Ctrl+C).
 * It gracefully stops the monitoring threads and ensures that the application remains responsive.
 *
 * @param sig The signal number (should be SIGINT).
 */
void handleSigint(int sig);

/**
 * @brief Prints the help menu to the console.
 *
 * Displays a list of available commands along with their descriptions to assist the user.
 */
void printHelp();

#endif // COMMAND_HANDLER_H
