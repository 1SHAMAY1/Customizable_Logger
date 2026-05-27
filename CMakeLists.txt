cmake_minimum_required(VERSION 3.10)
project(Customizable_Logger)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(Include)

add_library(Customizable_Logger STATIC
    Source/CustomizableLogger.cpp
)

set_target_properties(Customizable_Logger PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)
