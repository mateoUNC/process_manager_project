#include "utils.h"
#include <pwd.h>

// Utility function to get username from UID
std::string getUserNameFromUid(int uid) {
    struct passwd *pw = getpwuid(uid);
    if (pw) {
        return std::string(pw->pw_name);
    }
    return "Unknown";
}
