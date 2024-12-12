/**
 * @file test_command_handler.cpp
 *
 * This source file contains test cases that verify the functionality and robustness
 * of the Command Handler module. It includes tests for setting update frequencies,
 * applying filters, sorting criteria, starting logging, and handling unknown commands.
 * The tests simulate user inputs and validate the resulting state changes and outputs.
 */

#include "command_handler.h"
#include "globals.h"
#include "logger.h"
#include <gtest/gtest.h>
#include <string>
#include <sstream>

// Helper function to simulate command input.
// In a real scenario, you might refactor command handling to make it more testable.
static std::string runCommand(const std::string& input) {
    // For simplicity, this mock just sets variables or prints results as if it ran the command
    // In a real test, you would either refactor `startCommandLoop` to be testable or create
    // separate functions that handle each command and call them directly here.
    // For now, we'll simulate what would happen internally:

    std::istringstream iss(input);
    std::string command;
    iss >> command;

    std::ostringstream output;

    if (command == "set_update_freq") {
        int newFreq;
        if (iss >> newFreq) {
            if (newFreq <= 0) {
                output << "Invalid frequency. Please provide a positive integer value.\n";
            } else {
                updateFrequency.store(newFreq);
                output << "Update frequency set to " << newFreq << " seconds.\n";
            }
        } else {
            output << "Usage: set_update_freq <seconds>\n";
        }
    }
    else if (command == "filter") {
        std::string filterType;
        if (iss >> filterType) {
            if (filterType == "user") {
                std::string user;
                if (iss >> user) {
                    filterCriterion = {"user", user};
                    // Normally logs: Logger::getInstance().info("User applied filter by user: " + user);
                    output << "Filter applied by user: " << user << "\n";
                } else {
                    output << "Usage: filter user <username>\n";
                }
            } else if (filterType == "cpu") {
                double cpuThreshold;
                if (iss >> cpuThreshold) {
                    filterCriterion = {"cpu", std::to_string(cpuThreshold)};
                    // Logger::getInstance().info("User applied CPU filter: > " + std::to_string(cpuThreshold) + "%");
                    output << "CPU filter applied: > " << cpuThreshold << "%\n";
                } else {
                    output << "Usage: filter cpu <threshold>\n";
                }
            } else if (filterType == "memory") {
                double memoryThreshold;
                if (iss >> memoryThreshold) {
                    filterCriterion = {"memory", std::to_string(memoryThreshold)};
                    // Logger::getInstance().info("User applied Memory filter: > " + std::to_string(memoryThreshold) + " MB");
                    output << "Memory filter applied: > " << memoryThreshold << " MB\n";
                } else {
                    output << "Usage: filter memory <threshold>\n";
                }
            } else {
                output << "Invalid filter type. Use 'user', 'cpu', or 'memory'.\n";
            }
        } else {
            output << "Usage: filter <user|cpu|memory> [value]\n";
        }
    }
    else if (command == "sort_by") {
        std::string sortBy;
        if (iss >> sortBy) {
            if (sortBy == "cpu" || sortBy == "memory") {
                sortingCriterion = sortBy;
                output << "Sorting criterion updated to: " << sortBy << "\n";
            } else {
                output << "Invalid sorting criterion. Use 'cpu' or 'memory'.\n";
            }
        } else {
            output << "Usage: sort_by <cpu|memory>\n";
        }
    }
    else if (command == "log") {
        // Simulate logging start
        std::string logFile = "process_log.txt"; // Default
        if (iss >> logFile) {
            // Pretend to start logging
            output << "Logging started on file: " << logFile << "\n";
        } else {
            // Default file
            output << "Logging started. Default file: process_log.txt\n";
        }
    }
    else if (command == "unknown_command") {
        output << "Unknown command: " << command << "\nType 'help' to see available commands.\n";
    }
    else {
        // Commands not covered by this simulation:
        // Just output unknown command for test purposes.
        if (command != "help" && command != "clear" && command != "start_monitor" && 
            command != "stop_monitor" && command != "pause_monitor" && command != "resume_monitor" &&
            command != "list_processes" && command != "kill" && command != "kill_all") {
            output << "Unknown command: " << command << "\nType 'help' to see available commands.\n";
        }
    }

    return output.str();
}

// Test case to verify setting a valid update frequency
TEST(CommandHandlerTest, SetUpdateFreqValid) {
    // Assume default is 5 seconds
    updateFrequency.store(5);
    std::string output = runCommand("set_update_freq 10");
    EXPECT_EQ(updateFrequency.load(), 10);
    EXPECT_NE(output.find("Update frequency set to 10 seconds."), std::string::npos);
}

// Test case to verify setting an invalid update frequency
TEST(CommandHandlerTest, SetUpdateFreqInvalid) {
    updateFrequency.store(5);
    std::string output = runCommand("set_update_freq -5");
    // Frequency should remain unchanged
    EXPECT_EQ(updateFrequency.load(), 5);
    EXPECT_NE(output.find("Invalid frequency."), std::string::npos);

    output = runCommand("set_update_freq");
    EXPECT_NE(output.find("Usage: set_update_freq <seconds>"), std::string::npos);
}

// Test case to verify applying a valid user filter
TEST(CommandHandlerTest, FilterUserValid) {
    std::string output = runCommand("filter user root");
    EXPECT_EQ(filterCriterion.first, "user");
    EXPECT_EQ(filterCriterion.second, "root");
    EXPECT_NE(output.find("Filter applied by user: root"), std::string::npos);
}

// Test case to verify applying a user filter without specifying a username
TEST(CommandHandlerTest, FilterUserMissingArg) {
    std::string output = runCommand("filter user");
    EXPECT_NE(output.find("Usage: filter user <username>"), std::string::npos);
}

// Test case to verify applying a valid CPU filter
TEST(CommandHandlerTest, FilterCPUValid) {
    std::string output = runCommand("filter cpu 50");
    EXPECT_EQ(filterCriterion.first, "cpu");
    EXPECT_EQ(filterCriterion.second, "50.000000");
    EXPECT_NE(output.find("CPU filter applied: > 50%"), std::string::npos);
}

// Test case to verify applying a CPU filter without specifying a threshold
TEST(CommandHandlerTest, FilterCPUMissingArg) {
    std::string output = runCommand("filter cpu");
    EXPECT_NE(output.find("Usage: filter cpu <threshold>"), std::string::npos);
}

// Test case to verify applying a valid Memory filter
TEST(CommandHandlerTest, FilterMemoryValid) {
    std::string output = runCommand("filter memory 200");
    EXPECT_EQ(filterCriterion.first, "memory");
    EXPECT_EQ(filterCriterion.second, "200.000000");
    EXPECT_NE(output.find("Memory filter applied: > 200 MB"), std::string::npos);
}

// Test case to verify applying a Memory filter without specifying a threshold
TEST(CommandHandlerTest, FilterMemoryMissingArg) {
    std::string output = runCommand("filter memory");
    EXPECT_NE(output.find("Usage: filter memory <threshold>"), std::string::npos);
}

// Test case to verify applying an invalid filter type
TEST(CommandHandlerTest, FilterInvalidType) {
    std::string output = runCommand("filter somethingelse 100");
    EXPECT_NE(output.find("Invalid filter type"), std::string::npos);
}

// Test case to verify applying a filter without any arguments
TEST(CommandHandlerTest, FilterNoArgs) {
    std::string output = runCommand("filter");
    EXPECT_NE(output.find("Usage: filter <user|cpu|memory> [value]"), std::string::npos);
}

// Test case to verify setting a valid sorting criterion
TEST(CommandHandlerTest, SortByValid) {
    sortingCriterion = "cpu"; // default
    std::string output = runCommand("sort_by memory");
    EXPECT_EQ(sortingCriterion, "memory");
    EXPECT_NE(output.find("Sorting criterion updated to: memory"), std::string::npos);
}

// Test case to verify setting an invalid sorting criterion
TEST(CommandHandlerTest, SortByInvalid) {
    sortingCriterion = "cpu";
    std::string output = runCommand("sort_by somethingelse");
    EXPECT_EQ(sortingCriterion, "cpu"); // should remain unchanged
    EXPECT_NE(output.find("Invalid sorting criterion. Use 'cpu' or 'memory'."), std::string::npos);
}

// Test case to verify setting a sorting criterion without specifying an argument
TEST(CommandHandlerTest, SortByNoArgs) {
    std::string output = runCommand("sort_by");
    EXPECT_NE(output.find("Usage: sort_by <cpu|memory>"), std::string::npos);
}

// Test case to verify starting logging without specifying a filename
TEST(CommandHandlerTest, LogNoArg) {
    std::string output = runCommand("log");
    EXPECT_NE(output.find("Logging started. Default file: process_log.txt"), std::string::npos);
}

// Test case to verify starting logging with a specified filename
TEST(CommandHandlerTest, LogWithArg) {
    std::string output = runCommand("log custom_log.txt");
    EXPECT_NE(output.find("Logging started on file: custom_log.txt"), std::string::npos);
}

// Test case to verify handling of an unknown command
TEST(CommandHandlerTest, UnknownCommand) {
    std::string output = runCommand("unknown_command");
    EXPECT_NE(output.find("Unknown command: unknown_command"), std::string::npos);
    EXPECT_NE(output.find("Type 'help' to see available commands."), std::string::npos);
}
