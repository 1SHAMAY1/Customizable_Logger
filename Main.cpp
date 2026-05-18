#include "CustomizableLogger.hpp"
#include <thread>
#include <vector>

int main() {
    // Initialize the logger with asynchronous JSON file sink enabled
    CustomizableLogger logger(true, "log.json");

    // Register custom log levels integrated with ANSI terminal styling
    logger.registerLevel("GAMEPLAY/AI", "\033[36m");  // Cyan
    logger.registerLevel("UI/CLICK", "\033[35m");     // Magenta

    // Record application initialization stage
    LOG_INFO(logger, "System", "Program started. Starting multi-threaded stress test...");

    // Spin up concurrent worker threads to validate asynchronous and thread-safe operations under load
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

    // Join caller threads once all submissions are dispatched
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    // Record conclusion of stress-testing sequence
    LOG_INFO(logger, "System", "All writer threads completed their log tasks. Flushing queue...");

    // Block and synchronously wait for background queue to fully drain to all sinks
    logger.flush();

    return 0;
}
