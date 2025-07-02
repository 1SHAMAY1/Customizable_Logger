cmake_minimum_required(VERSION 3.10)
project(Customizable_Logger)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(Include)

add_executable(Customizable_Logger
    main.cpp
    Source/CustomizableLogger.cpp
)

set_target_properties(Customizable_Logger PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
