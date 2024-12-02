cmake_minimum_required(VERSION 3.10)
project(ProcessManager)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(process_manager_project
    src/main.cpp
    src/process_manager.cpp
    src/resource_monitor.cpp
    src/logger.cpp
    src/utils.cpp
)

# Otros ajustes de compilación si es necesario
