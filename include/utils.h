/**
 * @file utils.h
 * @brief Declares utility functions for miscellaneous tasks.
 *
 * This header file provides function declarations for common utility operations such as
 * converting UIDs to usernames.
 */

#ifndef UTILS_H
#define UTILS_H

#include <string>

/**
 * @brief Converts a UID to its corresponding username.
 *
 * Looks up the system's user database to find the username associated with the given UID.
 *
 * @param uid The User ID to convert.
 * @return The corresponding username, or "Unknown" if it cannot be found.
 */
std::string getUserNameFromUid(int uid);

#endif // UTILS_H
