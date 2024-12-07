// globals.h
#ifndef GLOBALS_H
#define GLOBALS_H

#include <atomic>
#include <mutex>
#include <condition_variable>
#include <unordered_map>

extern std::atomic<bool> monitoringActive;
extern std::mutex coutMutex;
extern std::mutex processMutex;
extern std::condition_variable cv;
extern std::mutex cvMutex;

#endif // GLOBALS_H
