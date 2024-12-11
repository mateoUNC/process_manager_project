#include "resource_monitor.h"
#include "command_handler.h"
#include <iostream>
#include "logger.h"

int main() {
    if (!Logger::getInstance().start("process_manager.log")) {
        std::cerr << "Failed to start logger!" << std::endl;
    }
    // ... rest of your initialization
    // For example:
    Logger::getInstance().info("Process Manager started.");

    // Run your command loop
    startCommandLoop();

    // Before exiting:
    Logger::getInstance().info("Shutting down Process Manager.");
    Logger::getInstance().stop();
    return 0;
}
