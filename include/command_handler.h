/**
 * @file command_handler.h
 * @brief Handles user commands and manages the command loop for the Process Manager application.
 *
 * This header file declares functions, variables, and constants related to command handling,
 * including starting the command loop, handling signals, and displaying help information
 * to the user. It also includes support for command-line auto-completion.
 */

#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <string>
#include <thread>
#include <vector>

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
 * @brief Thread used for monitoring processes.
 *
 * This thread is responsible for executing the process monitoring logic.
 * It is controlled through the command loop.
 */
extern std::thread monitoringThread;

/**
 * @brief List of available commands for the command completer.
 *
 * This list contains all supported commands that the user can input
 * into the command-line interface. It is used for auto-completion.
 */
extern const std::vector<std::string> commands;

/**
 * @brief Generates command suggestions for auto-completion.
 *
 * This function is called repeatedly by the Readline library to generate
 * command suggestions based on user input.
 *
 * @param text The current text entered by the user.
 * @param state The state of the completion (0 for a new completion sequence).
 * @return A pointer to the next matching command, or `nullptr` if no more matches are found.
 */
char* commandGenerator(const char* text, int state);

/**
 * @brief Completes commands for the Readline library.
 *
 * Provides auto-completion support for the Process Manager command-line interface.
 *
 * @param text The current text entered by the user.
 * @param start The start position of the completion.
 * @param end The end position of the completion.
 * @return A list of possible completions, or `nullptr` if none are found.
 */
char** commandCompleter(const char* text, int start, int end);

/**
 * @brief Handles the SIGINT signal (Ctrl+C).
 *
 * Stops any active monitoring threads and ensures that the application
 * remains responsive after receiving the signal.
 *
 * @param sig The signal number (should be SIGINT).
 */
void handleSigint(int sig);

/**
 * @brief Prints the help menu to the console.
 *
 * Displays a list of available commands along with their descriptions
 * to assist the user in using the application.
 */
void printHelp();

/**
 * @brief Starts the command processing loop.
 *
 * Initiates the interactive command loop where users can input commands
 * to control the Process Manager.
 */
void startCommandLoop();

#endif // COMMAND_HANDLER_H
