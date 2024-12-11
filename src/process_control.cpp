#include <signal.h> // For kill()
#include <unistd.h> // For getpid()
#include <iostream> // For std::cerr
#include "process_control.h"
#include "globals.h"

bool killProcess(int pid) {
    if (pid <= 0) {
        std::cerr << "Error: Invalid PID (" << pid << ").\n";
        return false;
    }

    if (pid == getpid()) {
        std::cerr << "Error: Cannot kill the current process (PID=" << pid << ").\n";
        return false;
    }

    // Attempt to send SIGKILL to the process
    if (kill(pid, SIGKILL) == 0) {
        std::cout << "Process " << pid << " terminated successfully.\n";
        return true;
    } else {
        // Handle specific errors
        switch (errno) {
            case ESRCH:
                std::cerr << "Error: Process " << pid << " does not exist.\n";
                break;
            case EPERM:
                std::cerr << "Error: Insufficient permissions to kill process " << pid << ".\n";
                break;
            default:
                perror("Error killing process");
                break;
        }
        return false;
    }
}


bool killProcessesByCpu(double threshold) {
    bool anyKilled = false;
    int successCount = 0;
    int failureCount = 0;

    {
        std::lock_guard<std::mutex> lock(processMutex); // Ensure thread-safe access to `processes`
        for (const auto& [pid, process] : processes) {
            if (process.cpuUsage > threshold) {
                if (kill(pid, SIGKILL) == 0) {
                    std::cout << "Killed process " << pid << " (CPU: " << process.cpuUsage << "%)\n";
                    anyKilled = true;
                    successCount++;
                } else {
                    std::cerr << "Failed to kill process " << pid << ": ";
                    switch (errno) {
                        case ESRCH:
                            std::cerr << "Process does not exist.\n";
                            break;
                        case EPERM:
                            std::cerr << "Insufficient permissions.\n";
                            break;
                        default:
                            perror("Error");
                            break;
                    }
                    failureCount++;
                }
            }
        }
    }

    if (!anyKilled) {
        std::cout << "No processes found exceeding the CPU usage threshold.\n";
    } else {
        std::cout << "Summary: " << successCount << " processes killed, " << failureCount << " failed.\n";
    }

    return anyKilled;
}

bool killProcessesByUser(const std::string& username) {
    bool anyKilled = false;
    int successCount = 0;
    int failureCount = 0;

    {
        std::lock_guard<std::mutex> lock(processMutex);
        for (const auto& [pid, process] : processes) {
            if (process.user == username) {
                if (kill(pid, SIGKILL) == 0) {
                    std::cout << "Killed process " << pid << " (User: " << username << ")\n";
                    anyKilled = true;
                    successCount++;
                } else {
                    std::cerr << "Failed to kill process " << pid << ": ";
                    switch (errno) {
                        case ESRCH:
                            std::cerr << "Process does not exist.\n";
                            break;
                        case EPERM:
                            std::cerr << "Insufficient permissions.\n";
                            break;
                        default:
                            perror("Error");
                            break;
                    }
                    failureCount++;
                }
            }
        }
    }

    if (!anyKilled) {
        std::cout << "No processes found for user: " << username << "\n";
    } else {
        std::cout << "Summary: " << successCount << " processes killed, " << failureCount << " failed.\n";
    }

    return anyKilled;
}
