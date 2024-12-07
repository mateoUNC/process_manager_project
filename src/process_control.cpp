#include <signal.h> // For kill()
#include <unistd.h> // For getpid()
#include <iostream> // For std::cerr

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
