#include "globals.h"
#include <atomic>
#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>

void startLogging(const std::string& filename)
{
    static std::atomic<bool> loggingActive(false);

    if (loggingActive.load())
    {
        std::cout << "Logging is already active.\n";
        return;
    }

    loggingActive.store(true);
    std::thread([filename]() {
        std::ofstream logFile(filename, std::ios::out | std::ios::app);
        if (!logFile.is_open())
        {
            std::cerr << "Failed to open log file: " << filename << "\n";
            loggingActive.store(false);
            return;
        }

        while (loggingActive.load())
        {
            {
                std::lock_guard<std::mutex> lock(processMutex);
                for (const auto& [pid, process] : processes)
                {
                    logFile << "PID: " << process.pid << ", User: " << process.user << ", CPU: " << process.cpuUsage
                            << "%"
                            << ", Memory: " << process.memoryUsage << " MB"
                            << ", Command: " << process.command << "\n";
                }
            }
            logFile.flush();
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }

        logFile.close();
        std::cout << "Logging stopped.\n";
    }).detach();
}
