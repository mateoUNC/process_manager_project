/**
 * @file logger.h
 * @brief Declares the Logger class for handling application-wide logging.
 *
 * The Logger class implements a thread-safe singleton pattern to manage log messages
 * with different severity levels. It supports asynchronous logging to a specified file.
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <atomic>
#include <condition_variable>
#include <fstream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

/**
 * @enum LogLevel
 * @brief Enumeration of log severity levels.
 *
 * Defines various levels of logging severity to categorize log messages.
 */
enum class LogLevel
{
    INFO,    /**< Informational messages */
    WARNING, /**< Warning messages */
    ERROR,   /**< Error messages */
    CRITICAL /**< Critical error messages */
};

/**
 * @class Logger
 * @brief Singleton class responsible for managing log messages.
 *
 * The Logger class provides methods to start and stop logging, and to log messages
 * at different severity levels. It runs a dedicated thread to process log messages
 * asynchronously, ensuring that logging does not block the main application flow.
 */
class Logger
{
  public:
    /**
     * @brief Retrieves the singleton instance of the Logger.
     *
     * @return Reference to the single Logger instance.
     */
    static Logger& getInstance();

    /**
     * @brief Starts logging by opening the specified log file.
     *
     * Initializes the logging system by opening the given file and starting the logging thread.
     *
     * @param filename The name of the file to which log messages will be written.
     * @return `true` if logging was successfully started, `false` otherwise.
     */
    bool start(const std::string& filename);

    /**
     * @brief Stops logging gracefully.
     *
     * Signals the logging thread to finish processing any pending messages and closes the log file.
     */
    void stop();

    /**
     * @brief Logs a message with the specified log level.
     *
     * Queues the message for asynchronous logging. The message will be written to the log file
     * with a timestamp and appropriate severity prefix.
     *
     * @param level The severity level of the log message.
     * @param message The content of the log message.
     */
    void log(LogLevel level, const std::string& message);

    /**
     * @brief Logs an informational message.
     *
     * Convenience method for logging messages at the INFO level.
     *
     * @param message The informational message to log.
     */
    void info(const std::string& message);

    /**
     * @brief Logs a warning message.
     *
     * Convenience method for logging messages at the WARNING level.
     *
     * @param message The warning message to log.
     */
    void warning(const std::string& message);

    /**
     * @brief Logs an error message.
     *
     * Convenience method for logging messages at the ERROR level.
     *
     * @param message The error message to log.
     */
    void error(const std::string& message);

    /**
     * @brief Logs a critical error message.
     *
     * Convenience method for logging messages at the CRITICAL level.
     *
     * @param message The critical error message to log.
     */
    void critical(const std::string& message);

  private:
    /**
     * @brief Private constructor to enforce singleton pattern.
     *
     * Initializes internal state but does not start logging. Use `start()` to begin logging.
     */
    Logger();

    /**
     * @brief Destructor that ensures logging is stopped gracefully.
     */
    ~Logger();

    // Delete copy constructor and assignment operator to prevent multiple instances
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    /**
     * @brief Processes the queued log messages in a dedicated thread.
     *
     * Continuously monitors the message queue and writes log entries to the file.
     * Terminates when `m_active` is set to `false`.
     */
    void processQueue();

    /**
     * @brief Formats a log message with a timestamp and severity level.
     *
     * Constructs a formatted string that includes the current timestamp, the log level,
     * and the message content.
     *
     * @param level The severity level of the log message.
     * @param message The content of the log message.
     * @return A formatted log message string.
     */
    std::string formatLogMessage(LogLevel level, const std::string& message);

    /**
     * @brief Converts a LogLevel enum to its corresponding string representation.
     *
     * @param level The LogLevel to convert.
     * @return A string representing the log level (e.g., "INFO", "WARNING").
     */
    std::string logLevelToString(LogLevel level);

    std::ofstream m_logFile;         /**< Output file stream for logging. */
    std::atomic<bool> m_active;      /**< Atomic flag indicating if logging is active. */
    std::thread m_logThread;         /**< Thread dedicated to processing log messages. */
    std::mutex m_mutex;              /**< Mutex for synchronizing start/stop operations. */
    std::mutex m_queueMutex;         /**< Mutex for protecting access to the message queue. */
    std::condition_variable m_cv;    /**< Condition variable for notifying the logging thread. */
    std::queue<std::string> m_queue; /**< Queue holding pending log messages. */
};

#endif // LOGGER_H
