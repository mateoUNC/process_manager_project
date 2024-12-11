#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <fstream>

enum class LogLevel {
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

class Logger {
public:
    static Logger& getInstance();

    // Start logging to a specific file
    bool start(const std::string& filename);

    // Stop logging gracefully
    void stop();

    // Log a message with a given log level
    void log(LogLevel level, const std::string& message);

    // Helper functions for convenience
    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);
    void critical(const std::string& message);

private:
    Logger();
    ~Logger();

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    // This function runs in a dedicated thread to process the message queue
    void processQueue();

    // Format the log message with a timestamp and log level
    std::string formatLogMessage(LogLevel level, const std::string& message);
    std::string logLevelToString(LogLevel level);

    std::ofstream m_logFile;
    std::atomic<bool> m_active;
    std::thread m_logThread;
    std::mutex m_mutex; // For starting/stopping
    std::mutex m_queueMutex;
    std::condition_variable m_cv;
    std::queue<std::string> m_queue;
};

#endif // LOGGER_H
