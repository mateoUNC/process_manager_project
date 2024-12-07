cmake_minimum_required(VERSION 3.10)
project(ProcessManager)

set(CMAKE_CXX_STANDARD 17)

# Include the directory containing header files
include_directories(include)

# Add executable with all source files
add_executable(process_manager_project
    src/main.cpp
    src/resource_monitor.cpp
    src/logger.cpp
    src/utils.cpp
    src/process_info.cpp
    src/process_display.cpp
    src/process_control.cpp
    src/command_handler.cpp
    src/globals.cpp
)

# Link the readline library
target_link_libraries(process_manager_project readline)
