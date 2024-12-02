#include "process_manager.h"
#include <iostream>

int main() {
    std::vector<Process> processes = getActiveProcesses();

    std::cout << "List of active processes:\n";
    for (const auto& process : processes) {
        std::cout << "PID: " << process.pid << ", Name: " << process.name << std::endl;
    }

    return 0;
}
