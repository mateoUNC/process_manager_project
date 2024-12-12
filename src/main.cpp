// main.cpp

/**
 * @file main.cpp
 *
 * This source file contains the `main` function, which serves as the entry point for the
 * Process Manager application. It initializes essential components such as the Logger,
 * starts the resource monitoring mechanisms, and initiates the command handling loop.
 * Proper initialization and graceful shutdown procedures are implemented to ensure that
 * all resources are managed correctly and that the application logs significant events
 * throughout its lifecycle.
 */

#include "command_handler.h"
#include "logger.h"
#include "resource_monitor.h"
#include <iostream>

/**
 * @brief The main function initializes the application and starts the command loop.
 *
 * The `main` function performs the following steps:
 * 1. Initializes and starts the Logger to record application events.
 * 2. Logs the startup event.
 * 3. Initializes resource monitoring threads for CPU and memory usage.
 * 4. Starts the command handling loop to process user inputs.
 * 5. Upon termination, logs the shutdown event and stops the Logger.
 *
 * @return Returns 0 upon successful execution.
 */
int main()
{
    // Initialize and start the Logger to record events to "process_manager.log"
    if (!Logger::getInstance().start("process_manager.log"))
    {
        std::cerr << "Failed to start logger!" << std::endl;
        return 1; // Exit with an error code if the logger fails to start
    }

    // Log that the Process Manager has started successfully
    Logger::getInstance().info("Process Manager started.");

    // Start the command handling loop to process user commands
    // This function blocks until the user decides to exit the application
    startCommandLoop();

    // Log that the Process Manager is shutting down
    Logger::getInstance().info("Shutting down Process Manager.");

    // Stop the Logger to ensure all logs are flushed and resources are released
    Logger::getInstance().stop();

    return 0; // Indicate successful execution
}
