### 1. Main Module (main.cpp)
**Responsibility:**
- **Program Entry Point**: Contains the `main()` function, which serves as the starting point of the application.
- **Command-Line Parsing**: Interprets user command-line arguments to decide which action to take (e.g., listing processes, monitoring resources, terminating a process).
- **Coordinator**: Instantiates other modules and coordinates their interactions by calling the appropriate functions based on user input.

**Key Functions:**
- **`int main(int argc, char* argv[])`**:
  - Parses command-line arguments to determine the requested action.
  - Invokes functions from other modules such as `monitorProcesses()` for resource monitoring.
  - Handles actions like:
    - `./process_manager list`: List processes.
    - `./process_manager monitor`: Start monitoring CPU and memory usage.
    - `./process_manager terminate <PID>`: Terminate a specific process.

### 2. Process Information Module (process_info.h and process_info.cpp)
**Responsibility:**
- **Process Data Retrieval**: Handles fetching detailed information about processes, including their IDs, users, commands, and memory usage.
- **Process Enumeration**: Provides a function to retrieve a list of active processes.
- **Encapsulation of Process Data**: Defines the `Process` struct, encapsulating all relevant process information.

**Key Functions:**
- **`std::vector<Process> getActiveProcesses()`**:
  - Retrieves a list of active processes by reading the `/proc` directory.
  - Populates the `Process` struct with details like PID, user, command, and memory usage.
- **`std::string getProcessUser(int pid)`**: Retrieves the username associated with a process's UID.
- **`std::string getProcessCommand(int pid)`**: Retrieves the command name of the process from `/proc/[pid]/comm`.
- **`double getProcessMemoryUsage(int pid)`**: Retrieves the memory usage of the process by reading `/proc/[pid]/status`.

**Process Struct**:
- Contains fields for `pid`, `user`, `cpuUsage`, `memoryUsage`, `prevTotalTime`, and `command`.

### 3. Resource Monitoring Module (resource_monitor.h and resource_monitor.cpp)
**Responsibility:**
- **Resource Monitoring Loop**: Manages the continuous monitoring of system resources, such as CPU and memory usage, for all active processes.
- **CPU Usage Calculation**: Calculates CPU usage for each process over time.
- **Data Aggregation**: Collects and updates process information at regular intervals.

**Key Functions:**
- **`void monitorProcesses()`**:
  - Runs an infinite loop that updates and displays process information periodically.
  - Coordinates the retrieval of process data and CPU usage calculations.
  - Clears the screen and displays updated information in a formatted table.
- **`long getTotalCpuTime()`**:
  - Reads and calculates the total CPU time from `/proc/stat`.
  - Includes all CPU times (user, system, idle, etc.).
- **`long getProcessTotalTime(int pid)`**:
  - Retrieves the total CPU time used by a specific process from `/proc/[pid]/stat`.
- **`double calculateCpuUsage(long processTimeDelta, long totalCpuTimeDelta, long numCores)`**:
  - Calculates the CPU usage percentage for a process, adjusting for the number of CPU cores.

**Multithreading Considerations**:
- Future enhancements could include multithreading to improve performance and responsiveness.

### 4. Process Display Module (process_display.h and process_display.cpp)
**Responsibility:**
- **User Interface Presentation**: Manages the formatting and display of process information to the console.
- **Output Formatting**: Ensures that the displayed data is neatly formatted, aligned, and easy to read.
- **Data Truncation and Alignment**: Handles the truncation of long strings and aligns columns appropriately.

**Key Functions:**
- **`void printProcesses(const std::vector<Process>& processes)`**:
  - Prints a table of processes, including headers and separators.
  - Formats columns for PID, User, CPU (%), Memory (MB), and Command.
  - Limits the number of displayed processes (e.g., top 30 by CPU usage).
  - Uses manipulators like `std::setw`, `std::left`, and `std::right` for alignment.

### 5. Utility Module (utils.h and utils.cpp)
**Responsibility:**
- **General-Purpose Utilities**: Provides helper functions that can be used across multiple modules.
- **User Information Retrieval**: Contains functions related to user data, such as mapping UIDs to usernames.

**Key Functions:**
- **`std::string getUserNameFromUid(int uid)`**: Converts a user ID (UID) to a username by querying the system's user database.

**Note**: Additional utility functions for thread management, data handling, or other common tasks can be added as needed.

### 6. Process Control Module (process_control.cpp and process_control.h)
**Responsibility:**
- **Process Termination**: Provides functionality to terminate processes based on a given PID.
- **Permission Management**: Ensures that the user has the necessary permissions to terminate the specified process.
- **Error Handling**: Handles errors such as invalid PIDs or insufficient permissions gracefully.

**Key Functions:**
- **`void terminateProcess(int pid)`**:
  - Attempts to terminate the process with the specified PID.
  - Uses system calls like `kill(pid, SIGTERM)` or `kill(pid, SIGKILL)` for termination.
  - Checks for and handles errors, such as invalid PIDs or lack of permissions.

**Implementation Status**:
- Ensure that this module is integrated with `main.cpp` for handling the terminate command.

### 7. Logging Module (logger.cpp and logger.h)
**Responsibility:**
- **Action Logging**: Records significant actions taken by the program, such as listing processes, monitoring resources, or terminating processes.
- **Error Logging**: Captures and logs errors encountered during execution for troubleshooting purposes.
- **Timestamping**: Adds timestamps to log entries to track when events occur.

**Key Functions:**
- **`void log(const std::string& message)`**:
  - Writes informational messages to a log file or standard output.
  - Includes a timestamp with each entry.
- **`void logError(const std::string& errorMessage)`**:
  - Specifically logs error messages.
  - Helps in diagnosing issues like failed process termination or permission errors.

**Implementation Status**:
- Logging can be integrated into other modules, especially `process_control` and `resource_monitor`, to record actions and errors.

