#include "CustomizableLogger.hpp"

int main() {
    // ✅ Initialize the logger with file output (writes to log.json)
    CustomizableLogger logger(true, "log.json");

    // ✅ Register custom log levels with ANSI colors
    // These will be used to color the logs in the console
    // "\033[36m" = Cyan, "\033[35m" = Magenta
    logger.registerLevel("GAMEPLAY/AI", "\033[36m");  // Cyan
    logger.registerLevel("UI/CLICK", "\033[35m");     // Magenta

    // ✅ Log the start of the program
    LOG_INFO(logger, "System", "Program started");

    // --- Optional: Filter which logs are shown ---
    // logger.setFilterLevels({"WARNING", "CRITICAL", "GAMEPLAY/AI"});
    // logger.setFilterCategories({"GAMEPLAY/"});
    //
    // - If filter levels are set, only logs matching those levels will be shown
    // - If filter categories are set, only logs with those category prefixes will be shown
    // - Both filters are optional — leave empty to show all logs

    // --- Sample Logs (Uncomment to test) ---

    // logger.log("System", "System booting...", "INFO");                  // Normal info log
    // logger.log("GAMEPLAY/AI", "AI is thinking...", "GAMEPLAY/AI");     // Custom level & category
    // logger.log("UI/CLICK", "Settings button clicked", "UI/CLICK");     // Custom level & category
    // logger.log("System", "Segfault detected", "CRITICAL");             // Critical system alert

    // ✅ Log the end of the program
    LOG_INFO(logger, "System", "Program ended");

    return 0;
}
