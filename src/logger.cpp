#include "logger.h"
#include <ctime>
#include <iomanip>
#include <sstream>

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

Logger::Logger() : m_active(false) {}

Logger::~Logger() {
    stop(); // Ensure logger is stopped at destruction
}

bool Logger::start(const std::string& filename) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_active.load()) {
        return false; // Already active
    }

    m_logFile.open(filename, std::ios::out | std::ios::app);
    if (!m_logFile.is_open()) {
        return false;
    }

    m_active.store(true);
    m_logThread = std::thread(&Logger::processQueue, this);
    return true;
}

void Logger::stop() {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_active.load()) return;
        m_active.store(false);
    }
    m_cv.notify_all();
    if (m_logThread.joinable()) {
        m_logThread.join();
    }
    if (m_logFile.is_open()) {
        m_logFile.close();
    }
}

void Logger::log(LogLevel level, const std::string& message) {
    if (!m_active.load()) return; // Logging not active
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        m_queue.push(formatLogMessage(level, message));
    }
    m_cv.notify_all();
}

void Logger::info(const std::string& message) {
    log(LogLevel::INFO, message);
}

void Logger::warning(const std::string& message) {
    log(LogLevel::WARNING, message);
}

void Logger::error(const std::string& message) {
    log(LogLevel::ERROR, message);
}

void Logger::critical(const std::string& message) {
    log(LogLevel::CRITICAL, message);
}

void Logger::processQueue() {
    while (true) {
        std::string logMsg;
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            m_cv.wait(lock, [this]() {
                return !m_active.load() || !m_queue.empty();
            });
            if (!m_active.load() && m_queue.empty()) {
                break; // Stop logging if inactive and queue is empty
            }

            logMsg = m_queue.front();
            m_queue.pop();
        }

        // Write the log message to the file
        m_logFile << logMsg << "\n";
        m_logFile.flush();
    }
}

std::string Logger::formatLogMessage(LogLevel level, const std::string& message) {
    auto now = std::time(nullptr);
    std::tm tm_buf;
    localtime_r(&now, &tm_buf);

    std::ostringstream oss;
    oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S") << " ["
        << logLevelToString(level) << "] " << message;
    return oss.str();
}

std::string Logger::logLevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::CRITICAL: return "CRITICAL";
        default: return "UNKNOWN";
    }
}
