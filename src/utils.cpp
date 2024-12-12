/**
 * @file utils.cpp
 * @description Implements utility functions for the Process Manager application.
 *
 * This source file contains helper functions that perform common tasks required
 * across different modules of the application. Specifically, it includes functions
 * to convert User IDs (UIDs) to their corresponding usernames by interfacing with
 * the system's user database.
 */

#include "utils.h"
#include "logger.h"
#include <pwd.h>

// Utility function to get username from UID
std::string getUserNameFromUid(int uid)
{
    // Retrieve password structure based on UID
    struct passwd* pw = getpwuid(uid);
    if (pw)
    {
        Logger::getInstance().warning("Unable to find username for UID: " + std::to_string(uid));
        return "Unknown";
    }
    // Return "Unknown" if the username cannot be determined
    return "Unknown";
}
