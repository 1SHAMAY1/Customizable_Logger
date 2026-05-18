#include "CustomizableLogger.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <cstring>
#include <cstdio>

CustomizableLogger::CustomizableLogger(bool toFile, const std::string& fileName)
    : fileOutput(toFile) {
    if (fileOutput) {
        logFile.open(fileName, std::ios::out | std::ios::app);
    }

    registerLevel("INFO", "\033[32m");
    registerLevel("WARNING", "\033[33m");
    registerLevel("ERROR", "\033[31m");
    registerLevel("DEBUG", "\033[34m");
    registerLevel("CRITICAL", "\033[41m");

    // Start background worker thread
    workerThread = std::thread(&CustomizableLogger::workerLoop, this);
}

CustomizableLogger::~CustomizableLogger() {
    running = false;
    cvNotEmpty.notify_all();
    cvNotFull.notify_all();
    
    if (workerThread.joinable()) {
        workerThread.join();
    }
    
    if (logFile.is_open()) {
        logFile.close();
    }
}

void CustomizableLogger::registerLevel(const std::string& lvl, const std::string& color) {
    std::lock_guard<std::mutex> lock(configMutex);
    logLevelColors[lvl] = color;
}

void CustomizableLogger::setFilterLevels(const std::vector<std::string>& levels) {
    std::lock_guard<std::mutex> lock(configMutex);
    filterLevels = levels;
}

void CustomizableLogger::setFilterCategories(const std::vector<std::string>& categories) {
    std::lock_guard<std::mutex> lock(configMutex);
    filterCategories = categories;
}

std::string CustomizableLogger::getColorNoLock(const std::string& level) {
    auto it = logLevelColors.find(level);
    return it != logLevelColors.end() ? it->second : "\033[0m";
}

std::string CustomizableLogger::getResetCode() {
    return "\033[0m";
}

void CustomizableLogger::fillTimestamp(char* buffer, size_t size) {
    auto now = std::chrono::system_clock::now();
    auto t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::tm localTime;
#if defined(_MSC_VER)
    localtime_s(&localTime, &t);  // Safe version for MSVC
#else
    localTime = *std::localtime(&t);  // Safe on POSIX
#endif

    char timeBuf[24];
    std::strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", &localTime);
    std::snprintf(buffer, size, "%s.%03d", timeBuf, static_cast<int>(ms.count()));
}

bool CustomizableLogger::passesFilterNoLock(const std::string& item,
                                             const std::vector<std::string>& filters) {
    if (filters.empty()) return true;
    for (const auto& f : filters) {
        if (item.rfind(f, 0) == 0) return true;
    }
    return false;
}

std::string CustomizableLogger::toJsonLine(const std::string& ts, const std::string& level,
                                           const std::string& cat, const std::string& msg) {
    std::ostringstream oss;
    oss << "{"
        << "\"timestamp\":\"" << ts << "\","
        << "\"level\":\"" << level << "\","
        << "\"category\":\"" << cat << "\","
        << "\"message\":\"" << msg << "\"}";
    return oss.str();
}

void CustomizableLogger::log(const std::string& category, const std::string& message,
                             const std::string& level) {
    // 1. Level and category filter verification under configuration lock
    {
        std::lock_guard<std::mutex> lock(configMutex);
        if (!passesFilterNoLock(level, filterLevels)) return;
        if (!passesFilterNoLock(category, filterCategories)) return;
    }

    // 2. Queue log entry inside the pre-allocated ring buffer
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        
        // Apply back-pressure if the ring buffer capacity is saturated
        cvNotFull.wait(lock, [this]() { return count < BUFFER_SIZE || !running; });
        if (!running) return;

        LogEntry& entry = ringBuffer[tail];

        // Format and store high-resolution timestamp (zero dynamic allocations)
        fillTimestamp(entry.timestamp, sizeof(entry.timestamp));

        // Perform bounded string copies to avoid heap allocation on the hot path
        std::strncpy(entry.level, level.c_str(), sizeof(entry.level) - 1);
        entry.level[sizeof(entry.level) - 1] = '\0';

        std::strncpy(entry.category, category.c_str(), sizeof(entry.category) - 1);
        entry.category[sizeof(entry.category) - 1] = '\0';

        std::strncpy(entry.message, message.c_str(), sizeof(entry.message) - 1);
        entry.message[sizeof(entry.message) - 1] = '\0';

        tail = (tail + 1) % BUFFER_SIZE;
        count++;

        cvNotEmpty.notify_one();
    }
}

void CustomizableLogger::flush() {
    std::unique_lock<std::mutex> lock(queueMutex);
    cvDrained.wait(lock, [this]() { return count == 0; });
}

void CustomizableLogger::workerLoop() {
    while (running) {
        LogEntry entry;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            cvNotEmpty.wait(lock, [this]() { return count > 0 || !running; });

            if (count == 0 && !running) {
                break;
            }

            entry = ringBuffer[head];
            head = (head + 1) % BUFFER_SIZE;
            count--;

            cvNotFull.notify_one();
            
            if (count == 0) {
                cvDrained.notify_all();
            }
        }
        processEntry(entry);
    }

    // Clean up any remaining logs after shutdown is initiated
    while (true) {
        LogEntry entry;
        bool hasItem = false;
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            if (count > 0) {
                entry = ringBuffer[head];
                head = (head + 1) % BUFFER_SIZE;
                count--;
                hasItem = true;
            }
        }
        if (!hasItem) break;
        processEntry(entry);
    }
    
    cvDrained.notify_all();
}

void CustomizableLogger::processEntry(const LogEntry& entry) {
    std::string color;
    {
        std::lock_guard<std::mutex> lock(configMutex);
        color = getColorNoLock(entry.level);
    }
    std::string reset = getResetCode();
    
    std::string formatted = "[" + std::string(entry.timestamp) + "] [" + 
                            std::string(entry.level) + "] [" + 
                            std::string(entry.category) + "] " + 
                            std::string(entry.message);

    std::cout << color << formatted << reset << std::endl;

    if (fileOutput && logFile.is_open()) {
        logFile << toJsonLine(entry.timestamp, entry.level, entry.category, entry.message) << "\n";
        logFile.flush();
    }
}
