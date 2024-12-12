#include "command_handler.h"
#include "logger.h"
#include "resource_monitor.h"
#include <iostream>

int main()
{
    if (!Logger::getInstance().start("process_manager.log"))
    {
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
