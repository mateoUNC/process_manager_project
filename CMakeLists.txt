cmake_minimum_required(VERSION 3.10)
project(ProcessManager)

set(CMAKE_CXX_STANDARD 17)
cmake_policy(SET CMP0135 NEW) # Opt into the NEW behavior explicitly

# Include the directory containing header files
include_directories(include)

# Add executable with all source files for the main application
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
    src/logging.cpp
    src/logger.cpp
)

# Link the readline library
target_link_libraries(process_manager_project readline)

# Enable testing
enable_testing()

# Download and configure Google Test if not already available
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.tar.gz
)
# Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(run_tests
    test/test_main.cpp
    test/test_resource_monitor.cpp
    test/test_process_info.cpp
    test/test_concurrent_updates.cpp
    test/test_process_control.cpp
    src/resource_monitor.cpp
    src/logger.cpp
    src/utils.cpp
    src/process_info.cpp
    src/process_display.cpp
    src/process_control.cpp
    src/globals.cpp
    src/logging.cpp
    src/logger.cpp
)

target_include_directories(run_tests PRIVATE
    ${CMAKE_BINARY_DIR}/build/_deps/googletest-src/googlemock/include
    ${CMAKE_BINARY_DIR}/build/_deps/googletest-src/googletest/include
)

target_link_libraries(run_tests PRIVATE GTest::gmock_main pthread readline)
target_compile_definitions(run_tests PRIVATE TESTING)
add_test(NAME ProcessManagerTests COMMAND run_tests)
