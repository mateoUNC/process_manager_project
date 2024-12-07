// globals.h
#ifndef GLOBALS_H
#define GLOBALS_H

#include <atomic>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include "process_info.h" // Include the Process struct and related functions

extern std::atomic<bool> monitoringActive;
extern std::mutex coutMutex;
extern std::mutex processMutex;
extern std::condition_variable cv;
extern std::mutex cvMutex;
extern std::string sortingCriterion;
extern std::unordered_map<int, Process> processes;

#endif // GLOBALS_H
