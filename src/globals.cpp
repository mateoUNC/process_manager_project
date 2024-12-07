// globals.cpp
#include "globals.h"

std::atomic<bool> monitoringActive(false);
std::mutex coutMutex;
std::mutex processMutex;
std::condition_variable cv;
std::mutex cvMutex;
std::string sortingCriterion = "cpu"; // Default sorting criterion
