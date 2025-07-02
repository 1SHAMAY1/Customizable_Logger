# 📝 Customizable Logger

A **lightweight, customizable C++ logging system** designed for console applications and developers who want:

- 🌈 Color-coded log levels
- 📁 JSON output to file
- 🔍 Runtime filtering by level and category
- 🧩 Support for user-defined log types (like gameplay tags)
- ⚡ Easy plug-and-play integration

---

## 📦 Features

- 🔧 Custom log levels with ANSI color codes
- 🏷️ Tag-style log levels and categories (e.g., GAMEPLAY/AI)
- 🎯 Runtime filters for log levels and categories
- 📄 Optional structured logging to `log.json`
- ⌚ Timestamped log entries (accurate to milliseconds)
- 🧪 Works out-of-the-box with CMake

---

## 📁 Folder Structure

Customizable_Logger/
├── Include/
│   └── CustomizableLogger.hpp       # Logger class header
├── Source/
│   └── CustomizableLogger.cpp       # Logger class implementation
├── main.cpp                         # Sample usage (entry point)
├── bin/                             # Final .exe copied here
├── Build/                           # Cleaned/rebuilt each run
├── CMakeLists.txt                   # CMake build file
├── build_and_run_logger.bat         # 🔁 One-click build & run
├── log.json                         # (Generated log output file)
└── README.md                        # 📘 You're here!

---

## 🚀 Quick Start

### ✅ 1. Requirements

- Windows + Visual Studio 2022
- CMake (v3.20 or newer) installed and added to PATH
- A terminal that supports ANSI colors (e.g., Windows Terminal or PowerShell)

---

### 🔄 2. Build & Run Automatically (Recommended)

Run the provided batch file:

    build_and_run_logger.bat

This will:
- Clean the previous build
- Run CMake and MSBuild
- Copy the executable to `/bin/`
- Run the logger
- Show output in console and `log.json`

---

### 🛠️ 3. Manual Build (Alternative)

    cd Customizable_Logger
    mkdir Build
    cd Build
    cmake ..
    cmake --build .
    ./bin/Debug/Customizable_Logger.exe

---

## 🧪 Usage Example

### main.cpp

```cpp
#include "CustomizableLogger.hpp"

int main() {
    CustomizableLogger logger(true, "log.json");

    logger.registerLevel("GAMEPLAY/AI", "\033[36m");  // Cyan
    logger.registerLevel("UI/CLICK", "\033[35m");     // Magenta

    // Filters (optional)
    // logger.setFilterLevels({"WARNING", "CRITICAL", "GAMEPLAY/AI"});
    // logger.setFilterCategories({"GAMEPLAY/"});

    logger.log("System", "Program started", "INFO");

    // Sample logs (uncomment to test)
    // logger.log("System", "System booting...", "INFO");
    // logger.log("GAMEPLAY/AI", "AI is thinking...", "GAMEPLAY/AI");
    // logger.log("UI/CLICK", "Settings button clicked", "UI/CLICK");
    // logger.log("System", "Segfault detected", "CRITICAL");

    logger.log("System", "Program ended", "INFO");

    return 0;
}


## 🔧 Customization

### 🎨 Color Codes (ANSI)
| Color     | ANSI Code  |
|-----------|------------|
| Red       | `\033[31m` |
| Green     | `\033[32m` |
| Yellow    | `\033[33m` |
| Blue      | `\033[34m` |
| Magenta   | `\033[35m` |
| Cyan      | `\033[36m` |

Use them like this:

```cpp
logger.registerLevel("MY_TAG", "\033[33m");  // Yellow
```

---

### 🎚️ Log Filters

You can restrict log output by **log level** and/or **category prefix**.

```cpp
logger.setFilterLevels({"CRITICAL", "GAMEPLAY/AI"});
logger.setFilterCategories({"GAMEPLAY/", "SYSTEM"});
```

- If left empty, all levels and categories are allowed.
- Filtering uses prefix-matching (e.g., `GAMEPLAY/` matches `GAMEPLAY/AI`).

---

## 📄 Output Format

### ✅ Console Output (colored)
```
[2025-07-02 18:30:42.123] [CRITICAL] [System] Segfault detected
```

### 📝 JSON File Output (`log.json`)
```json
{
  "timestamp": "2025-07-02 18:30:42.123",
  "level": "CRITICAL",
  "category": "System",
  "message": "Segfault detected"
}
```

---

## 🧩 Advanced Ideas

- ⏱️ Track time since last log
- 🧵 Include thread IDs
- 🧹 Rotate/auto-clean old logs
- 🖥️ Visual log viewer GUI
- 🐛 Debug-only logger with macro wrapping

---

## 📃 License

This project is licensed under the **MIT License**.  
You may freely use, modify, and distribute this for personal and commercial use.

---

## 🔗 Helpful Links

- [CMake Download](https://cmake.org/download/)
- [ANSI Color Codes (Wikipedia)](https://en.wikipedia.org/wiki/ANSI_escape_code)
- [MIT License](https://opensource.org/licenses/MIT)