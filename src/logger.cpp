/**
 * @file logger.cpp
 * @brief Implements the Logger class for handling application-wide logging.
 *
 * This source file contains the implementation of the Logger class, which follows the singleton
 * design pattern to manage log messages with different severity levels. It supports asynchronous
 * logging to a specified file by utilizing a dedicated logging thread and a message queue.
 * The Logger ensures thread-safe operations, allowing multiple threads to enqueue log messages
 * without data races or inconsistencies.
 */

#include "logger.h"
#include <ctime>
#include <iomanip>
#include <sstream>

// Retrieves the singleton instance of the Logger.
// Ensures that only one instance of Logger exists throughout the application.
Logger& Logger::getInstance()
{
    static Logger instance;
    return instance;
}

// Constructor initializes the Logger as inactive.
Logger::Logger() : m_active(false)
{
}

// Destructor ensures that the Logger is stopped gracefully upon destruction.
Logger::~Logger()
{
    stop(); // Ensure logger is stopped at destruction
}

// Starts the Logger by opening the specified log file and launching the logging thread.
// Returns false if the Logger is already active or if the log file cannot be opened.
bool Logger::start(const std::string& filename)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_active.load())
    {
        return false; // Already active
    }

    m_logFile.open(filename, std::ios::out | std::ios::app);
    if (!m_logFile.is_open())
    {
        return false;
    }

    m_active.store(true);
    m_logThread = std::thread(&Logger::processQueue, this); // Launch logging thread
    return true;
}

// Stops the Logger by signaling the logging thread to finish processing and closing the log file.
void Logger::stop()
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_active.load())
            return;
        m_active.store(false); // Signal logging thread to stop
    }
    m_cv.notify_all(); // Wake up the logging thread if it's waiting
    if (m_logThread.joinable())
    {
        m_logThread.join(); // Wait for the logging thread to finish
    }
    if (m_logFile.is_open())
    {
        m_logFile.close(); // Close the log file
    }
}

// Enqueues a log message with the specified log level.
// If logging is not active, the message is discarded.
void Logger::log(LogLevel level, const std::string& message)
{
    if (!m_active.load())
        return; // Logging not active
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        m_queue.push(formatLogMessage(level, message)); // Add formatted message to the queue
    }
    m_cv.notify_all(); // Notify the logging thread that a new message is available
}

// Convenience method to log an informational message.
void Logger::info(const std::string& message)
{
    log(LogLevel::INFO, message);
}

// Convenience method to log a warning message.
void Logger::warning(const std::string& message)
{
    log(LogLevel::WARNING, message);
}

// Convenience method to log an error message.
void Logger::error(const std::string& message)
{
    log(LogLevel::ERROR, message);
}

// Convenience method to log a critical error message.
void Logger::critical(const std::string& message)
{
    log(LogLevel::CRITICAL, message);
}

// Dedicated thread function that processes log messages from the queue.
// Continues running until the Logger is stopped and the queue is empty.
void Logger::processQueue()
{
    while (true)
    {
        std::string logMsg;
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            // Wait until there is a message to process or the Logger is stopped
            m_cv.wait(lock, [this]() { return !m_active.load() || !m_queue.empty(); });
            if (!m_active.load() && m_queue.empty())
            {
                break; // Exit the loop if Logger is inactive and no messages are left
            }

            logMsg = m_queue.front(); // Retrieve the next message
            m_queue.pop();            // Remove it from the queue
        }

        // Write the log message to the file
        m_logFile << logMsg << "\n";
        m_logFile.flush(); // Ensure the message is written to the file immediately
    }
}

// Formats a log message by adding a timestamp and converting the log level to a string.
// The formatted message includes the date, time, log level, and the actual message.
std::string Logger::formatLogMessage(LogLevel level, const std::string& message)
{
    auto now = std::time(nullptr);
    std::tm tm_buf;
    localtime_r(&now, &tm_buf); // Convert time to local time structure

    std::ostringstream oss;
    oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S") // Format: YYYY-MM-DD HH:MM:SS
        << " [" << logLevelToString(level) << "] "     // Add log level in brackets
        << message;                                    // Append the actual log message
    return oss.str();
}

// Converts a LogLevel enum value to its corresponding string representation.
// This is used to prefix log messages with their severity level.
std::string Logger::logLevelToString(LogLevel level)
{
    switch (level)
    {
    case LogLevel::INFO:
        return "INFO";
    case LogLevel::WARNING:
        return "WARNING";
    case LogLevel::ERROR:
        return "ERROR";
    case LogLevel::CRITICAL:
        return "CRITICAL";
    default:
        return "UNKNOWN";
    }
}
