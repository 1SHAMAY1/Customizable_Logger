#include "CustomizableLogger.hpp"
#include <thread>
#include <vector>

int main() {
    // ✅ Initialize the logger with file output (writes to log.json)
    CustomizableLogger logger(true, "log.json");

    // ✅ Register custom log levels with ANSI colors
    // These will be used to color the logs in the console
    // "\033[36m" = Cyan, "\033[35m" = Magenta
    logger.registerLevel("GAMEPLAY/AI", "\033[36m");  // Cyan
    logger.registerLevel("UI/CLICK", "\033[35m");     // Magenta

    // ✅ Log the start of the program
    LOG_INFO(logger, "System", "Program started. Starting multi-threaded stress test...");

    // ✅ Spawn multiple threads writing concurrently to demonstrate thread safety and async non-blocking execution
    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([&logger, i]() {
            for (int j = 0; j < 10; ++j) {
                std::string msg = "Async message " + std::to_string(j) + " from thread " + std::to_string(i);
                if (j % 3 == 0) {
                    LOG_CUSTOM(logger, "GAMEPLAY/AI", msg, "GAMEPLAY/AI");
                } else if (j % 3 == 1) {
                    LOG_WARNING(logger, "Physics", msg);
                } else {
                    LOG_INFO(logger, "Network", msg);
                }
            }
        });
    }

    // Wait for all caller threads to finish submitting their log requests
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    // ✅ Log the end of the program
    LOG_INFO(logger, "System", "All writer threads completed their log tasks. Flushing queue...");

    // ✅ Block and wait for all logs to be written to console & file before exiting
    logger.flush();

    return 0;
}
