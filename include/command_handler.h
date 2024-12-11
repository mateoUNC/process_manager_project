#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <string>

// Define ANSI color codes for styling
#define RESET "\033[0m"
#define CYAN "\033[36m"
#define YELLOW "\033[33m"
#define GREEN "\033[32m"
#define RED "\033[31m"
#define BOLD "\033[1m"

// Function declarations
void startCommandLoop();
void handleSigint(int sig); // Handles SIGINT signal (Ctrl+C)
void printHelp();           // Prints the help menu

#endif // COMMAND_HANDLER_H
