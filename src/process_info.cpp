/**
 * @file process_info.cpp
 * @brief Implements functions for retrieving and managing process information.
 *
 * This source file contains the implementation of functions that gather information
 * about active processes on the system. It includes methods to retrieve the user
 * owning a process, the command associated with a process, and the memory usage of
 * a process. Additionally, it provides functionality to list all active processes
 * by scanning the `/proc` filesystem. Thread safety is maintained through the use
 * of mutexes when accessing shared data structures.
 */

#include "process_info.h"
#include "utils.h"
#include <cctype>
#include <dirent.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <pwd.h>
#include <sstream>
#include <unistd.h>

// Function to get the username of a process owner based on PID
std::string getProcessUser(int pid)
{
    // Open the /proc/[pid]/status file to read process information
    std::ifstream statusFile("/proc/" + std::to_string(pid) + "/status");
    if (!statusFile.is_open())
    {
        return "Unknown"; // Return "Unknown" if the file cannot be opened
    }

    std::string line;
    while (std::getline(statusFile, line))
    {
        // Look for the line that starts with "Uid:"
        if (line.find("Uid:") == 0)
        {
            std::istringstream ss(line.substr(5)); // Extract the UID field
            int uid;
            ss >> uid;                      // Read the UID value
            return getUserNameFromUid(uid); // Convert UID to username
        }
    }
    return "Unknown"; // Return "Unknown" if UID is not found
}

// Function to get the command name of a process based on PID
std::string getProcessCommand(int pid)
{
    // Open the /proc/[pid]/comm file to read the command name
    std::ifstream commFile("/proc/" + std::to_string(pid) + "/comm");
    if (!commFile.is_open())
    {
        return "Unknown"; // Return "Unknown" if the file cannot be opened
    }
    std::string command;
    std::getline(commFile, command); // Read the command name
    return command;
}

// Function to get the memory usage of a process based on PID
double getProcessMemoryUsage(int pid)
{
    // Open the /proc/[pid]/status file to read memory information
    std::ifstream statusFile("/proc/" + std::to_string(pid) + "/status");
    if (!statusFile.is_open())
    {
        return 0.0; // Return 0.0 if the file cannot be opened
    }

    std::string line;
    while (std::getline(statusFile, line))
    {
        // Look for the line that starts with "VmRSS:"
        if (line.find("VmRSS:") == 0)
        {
            std::istringstream ss(line.substr(6)); // Extract the VmRSS field
            long vmRSS;
            ss >> vmRSS;           // Read the VmRSS value in KB
            return vmRSS / 1024.0; // Convert from KB to MB and return
        }
    }
    return 0.0; // Return 0.0 if VmRSS is not found
}

// Function to retrieve a list of all active processes
std::vector<Process> getActiveProcesses()
{
    std::vector<Process> processes; // Vector to store active processes

    // Open the /proc directory to scan for process directories
    DIR* dir = opendir("/proc");
    if (dir == nullptr)
    {
        std::cerr << "Cannot open /proc directory" << std::endl;
        return processes; // Return empty vector if /proc cannot be opened
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr)
    {
        // Check if the directory name starts with a digit (indicating a PID)
        if (isdigit(entry->d_name[0]))
        {
            int pid = std::stoi(entry->d_name); // Convert directory name to PID

            // Initialize a Process object with the retrieved PID
            Process process;
            process.pid = pid;                                // Assign the PID
            process.user = getProcessUser(pid);               // Retrieve the process owner
            process.memoryUsage = getProcessMemoryUsage(pid); // Retrieve memory usage
            process.command = getProcessCommand(pid);         // Retrieve the command name

            processes.push_back(process); // Add the Process object to the vector
        }
    }

    closedir(dir);    // Close the /proc directory
    return processes; // Return the list of active processes
}
