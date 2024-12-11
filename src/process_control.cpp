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
        // Process killed successfully
        return true;
    } else {
        // Handle errors (e.g., no such process or insufficient permissions)
        perror("Error killing process");
        return false;
    }
}

bool killProcessesByCpu(double threshold) {
    bool anyKilled = false;

    {
        std::lock_guard<std::mutex> lock(processMutex); // Ensure thread-safe access to `processes`
        for (const auto& [pid, process] : processes) {
            if (process.cpuUsage > threshold) {
                if (kill(pid, SIGKILL) == 0) {
                    std::cout << "Killed process " << pid << " (CPU: " << process.cpuUsage << "%)\n";
                    anyKilled = true;
                } else {
                    std::cerr << "Failed to kill process " << pid << ".\n";
                }
            }
        }
    }

    if (!anyKilled) {
        std::cout << "No processes found exceeding the CPU usage threshold.\n";
    }

    return anyKilled;
}

bool killProcessesByUser(const std::string& username) {
    bool anyKilled = false;

    {
        std::lock_guard<std::mutex> lock(processMutex);
        for (const auto& [pid, process] : processes) {
            if (process.user == username) {
                if (kill(pid, SIGKILL) == 0) {
                    std::cout << "Killed process " << pid << " (User: " << username << ")\n";
                    anyKilled = true;
                } else {
                    std::cerr << "Failed to kill process " << pid << ".\n";
                }
            }
        }
    }

    if (!anyKilled) {
        std::cout << "No processes found for user: " << username << "\n";
    }

    return anyKilled;
}
