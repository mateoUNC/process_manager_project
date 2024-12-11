// globals.cpp
#include "globals.h"

std::atomic<bool> monitoringActive(false);
std::mutex coutMutex;
std::mutex processMutex;
std::condition_variable cv;
std::mutex cvMutex;
std::string sortingCriterion = "cpu"; // Default sorting criterion
std::unordered_map<int, Process> processes; // Define the shared processes map
std::atomic<bool> monitoringPaused(false); // Default: not paused
std::pair<std::string, std::string> filterCriterion = { "none", "" }; // Default: no filter
