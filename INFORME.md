# Process Manager Documentation

## Introduction

The **Process Manager** is a comprehensive application designed to monitor and manage system processes on a Linux-based operating system. It provides users with a suite of tools to track resource usage, retrieve detailed information about active processes, and perform actions such as terminating processes. The primary aim of this program is to offer an intuitive and efficient interface for understanding and managing system performance, catering to both casual users and system administrators.

### Key Features:
- **Process Monitoring**: Continuously tracks active processes, displaying critical information such as CPU usage, memory consumption, and the command associated with each process.
- **Resource Usage Analysis**: Provides real-time calculations of CPU and memory usage, enabling users to identify resource-intensive processes.
- **Process Termination**: Allows users to safely and efficiently terminate processes by specifying their Process ID (PID).
- **User-Friendly Interface**: Displays process information in a clear, formatted table for easy readability.
- **Customizable Commands**: Supports command-line options for flexibility, enabling users to list processes, monitor resources, or manage specific processes.

### Use Cases:
1. **System Performance Monitoring**:
   - Detect and troubleshoot resource-intensive processes affecting system performance.
   - Gain insights into CPU and memory utilization across all active processes.

2. **System Administration**:
   - Monitor and manage server processes in real-time.
   - Identify and terminate misbehaving or unnecessary processes to maintain system stability.

3. **Development and Debugging**:
   - Observe the behavior of application processes during runtime.
   - Evaluate the resource footprint of custom applications under different conditions.

The Process Manager is built using modular components, each handling a distinct aspect of the program’s functionality, such as retrieving process data, monitoring resource usage, or managing user interactions. This documentation will guide you through the codebase, explaining each module's responsibilities, key functions, and integration points within the application.

# Process Control Module Documentation

## Overview

The **Process Control Module** provides the functionality to manage and terminate processes on the system. It allows for controlled termination of individual processes, as well as batch operations based on criteria such as CPU usage or user ownership. By integrating robust error handling mechanisms, the module ensures safe execution of these actions without disrupting system stability.

---

## Functions

### 1. `bool killProcess(int pid)`
#### Description
Attempts to terminate a process with the specified Process ID (PID). This function sends a `SIGKILL` signal to the target process and handles common errors to ensure the operation is performed safely.

#### Parameters
- `pid`: The Process ID of the process to terminate.

#### Returns
- `true`: If the process was successfully terminated.
- `false`: If the termination failed due to invalid PID, insufficient permissions, or other errors.

#### Key Details
- Validates the PID to ensure it corresponds to an active process.
- Prevents termination of the current process to avoid self-induced shutdowns.
- Logs any errors encountered during the operation.

---

### 2. `bool killProcessesByCpu(double threshold)`
#### Description
Terminates all processes whose CPU usage exceeds a specified percentage threshold. This is particularly useful for managing runaway processes that consume excessive CPU resources.

#### Parameters
- `threshold`: The CPU usage percentage above which processes will be terminated.

#### Returns
- `true`: If at least one process was terminated.
- `false`: If no processes met the criteria or if all attempts failed.

#### Key Details
- Iterates through the list of monitored processes to evaluate their CPU usage.
- Sends `SIGKILL` signals to processes exceeding the threshold.
- Logs the outcome of each termination attempt, including successes and failures.

---

### 3. `bool killProcessesByUser(const std::string& username)`
#### Description
Terminates all processes owned by a specific user. This function is helpful for managing processes tied to a particular user account.

#### Parameters
- `username`: The username whose processes will be terminated.

#### Returns
- `true`: If at least one process was terminated.
- `false`: If no processes matched the user or if all attempts failed.

#### Key Details
- Maps the username to a user ID (UID) for process identification.
- Iterates through monitored processes and sends `SIGKILL` signals to matching entries.
- Logs successes and errors for each termination attempt.

---

## Error Handling

The module includes robust error handling to address:
- **Invalid Process IDs**: Ensures that termination attempts are only made on valid PIDs.
- **Insufficient Permissions**: Verifies user privileges before attempting to kill processes.
- **System Integrity**: Prevents accidental termination of critical system processes or the current process.

---

## Integration Points

- **Main Module**: Integrates the process control functions for user commands like `terminate`.
- **Logging Module**: Logs all actions and errors for traceability and debugging purposes.
- **Resource Monitoring Module**: Supplies the process data needed for filtering and identifying target processes.

# Process Display Module Documentation

## Overview

The **Process Display Module** is responsible for presenting process information to the user in a structured and readable format. By applying sorting and filtering criteria, this module ensures that the most relevant process data is displayed clearly and effectively. This functionality is crucial for enabling users to monitor and manage processes efficiently.

---

## Functions

### 1. `void printProcesses(const std::vector<Process>& processes)`
#### Description
Displays a list of processes in a neatly formatted table. This function prints the details of each process, including:
- **Process ID (PID)**
- **User owning the process**
- **CPU usage percentage**
- **Memory usage in MB**
- **Command name**

#### Parameters
- `processes`: A vector of `Process` structs containing information about active processes. Each `Process` struct includes attributes such as PID, user, CPU usage, memory usage, and the associated command.

#### Key Details
- **Formatting**: Uses standardized column widths and alignment (e.g., `std::setw`, `std::left`, `std::right`) to ensure readability.
- **Sorting and Filtering**:
  - Processes can be sorted based on CPU or memory usage for prioritization.
  - Filters can exclude processes based on user-defined criteria (e.g., specific users or low resource usage).
- **Headers**:
  - Displays column headers (e.g., `PID`, `User`, `CPU (%)`, `Memory (MB)`, `Command`) for easy identification of data fields.
- **Pagination** (Optional): Limits the number of displayed processes, such as showing the top 30 by default.

#### Example Output
```
 PID   User      CPU (%)   Memory (MB)   Command
-------------------------------------------------
 1234  root      15.2      200.3         apache2
 5678  user1      8.5      102.1         chrome
 9101  user2      1.3       50.0         vim
```

---

## Integration Points

- **Resource Monitoring Module**: Supplies the process data (via `Process` structs) required for display.
- **Main Module**: Calls `printProcesses` to render output based on user commands (e.g., `list`).
- **Utility Module**: May provide additional helpers for text alignment or truncation of overly long strings.

---

## Error Handling

The module gracefully handles scenarios such as:
- **Empty Data**: Displays a message if no processes meet the filtering criteria.
- **Long Command Names**: Truncates excessively long command strings to fit within column limits while appending an ellipsis (`...`).
- **Invalid Data**: Skips over corrupted or incomplete `Process` entries, logging an error if necessary.

# Process Information Module Documentation

## Overview

The **Process Information Module** is responsible for retrieving and encapsulating detailed information about the active processes running on the system. This module provides functionality for process enumeration, data extraction, and encapsulation in a structured format, allowing other components of the application to easily access and utilize process data.

---

## Structures

### 1. `struct Process`
#### Description
Represents detailed information about a single process. This struct encapsulates key attributes of a process, enabling easy manipulation and display of its data.

#### Fields
- `int pid`: Process ID, a unique identifier for the process.
- `std::string user`: Username of the process owner.
- `double cpuUsage`: Percentage of CPU usage by the process.
- `double memoryUsage`: Memory usage of the process in megabytes (MB).
- `long prevTotalTime`: Previous total CPU time used by the process.
- `std::string command`: Command associated with the process.

---

## Functions

### 1. `std::vector<Process> getActiveProcesses()`
#### Description
Retrieves a list of all currently active processes by scanning the `/proc` filesystem. Each process's details are stored in a `Process` struct.

#### Returns
- `std::vector<Process>`: A vector containing `Process` structs for all active processes.

#### Key Details
- Reads system files like `/proc/[pid]/status` and `/proc/[pid]/comm` to gather process data.
- Handles errors gracefully, such as inaccessible or corrupted process directories.

---

### 2. `std::string getProcessUser(int pid)`
#### Description
Retrieves the username associated with a specific process by mapping the UID found in `/proc/[pid]/status` to a username.

#### Parameters
- `int pid`: The Process ID of the target process.

#### Returns
- `std::string`: The username of the process owner, or "Unknown" if it cannot be determined.

#### Key Details
- Utilizes system functions or file parsing to translate UIDs to usernames.
- Ensures fallback mechanisms for cases where usernames cannot be resolved.

---

### 3. `std::string getProcessCommand(int pid)`
#### Description
Retrieves the command name associated with a specific process by reading the `/proc/[pid]/comm` file.

#### Parameters
- `int pid`: The Process ID of the target process.

#### Returns
- `std::string`: The command name, or "Unknown" if it cannot be determined.

#### Key Details
- Handles cases where the `/proc/[pid]/comm` file is inaccessible or empty.

---

### 4. `double getProcessMemoryUsage(int pid)`
#### Description
Calculates the memory usage of a specific process by reading the `Resident Set Size (RSS)` field in `/proc/[pid]/status`.

#### Parameters
- `int pid`: The Process ID of the target process.

#### Returns
- `double`: The memory usage in megabytes (MB), or 0.0 if it cannot be determined.

#### Key Details
- Converts RSS values from kilobytes to megabytes for readability.
- Ensures fallback to default values if parsing errors occur.

---

## Integration Points

- **Resource Monitoring Module**: Utilizes this module to retrieve process data for monitoring CPU and memory usage.
- **Process Display Module**: Accesses `Process` structs to display information to the user in a formatted table.
- **Main Module**: Uses this module for handling user commands that require process details (e.g., listing or filtering processes).

---

## Error Handling

- **File Access Issues**: Gracefully skips processes with inaccessible `/proc` directories and logs appropriate warnings.
- **Invalid Data**: Ensures default values or error messages for corrupted or incomplete process data.
- **Performance**: Optimized to handle large numbers of processes efficiently.

---

# Resource Monitoring Module Documentation

## Overview

The **Resource Monitoring Module** provides the functionality to monitor system and process-level resource usage, including CPU and memory. It continuously tracks and updates resource metrics, ensuring that real-time data is available for system analysis and process management. This module is critical for identifying resource-intensive processes and understanding overall system performance.

---

## Functions

### 1. `void monitorProcesses()`
#### Description
Monitors and updates the list of active processes. Continuously scans for new or terminated processes, applies filtering and sorting criteria, and updates the global processes list with the latest information.

#### Key Details
- Integrates with the **Process Information Module** to retrieve process details.
- Updates the global list of processes by adding new entries and removing terminated ones.
- Provides the foundational data required for CPU and memory monitoring functions.

---

### 2. `void monitorCpu()`
#### Description
Periodically calculates the CPU usage for each monitored process by comparing the current and previous total CPU times. Updates the `cpuUsage` attribute of each process.

#### Key Details
- Relies on:
  - **`getTotalCpuTime`**: For system-wide CPU usage data.
  - **`getProcessTotalTime`**: For per-process CPU time data.
- Uses delta calculations to determine the CPU usage percentage for each process over a fixed interval.
- Adjusts results based on the number of CPU cores.

---

### 3. `void monitorMemory()`
#### Description
Periodically updates the memory usage attribute for each monitored process by reading the latest data from the system.

#### Key Details
- Integrates with the **Process Information Module** to fetch memory usage metrics.
- Ensures that updated memory usage is reflected in the `memoryUsage` attribute of each process.

---

### 4. `long getTotalCpuTime()`
#### Description
Retrieves the total CPU time across all cores by reading the `/proc/stat` file.

#### Returns
- `long`: The aggregate CPU time in jiffies, or 0 if it cannot be determined.

#### Key Details
- Includes all CPU states (user, system, idle, etc.) in the calculation.
- Handles errors such as missing or corrupted `/proc/stat` file gracefully.

---

### 5. `long getProcessTotalTime(int pid)`
#### Description
Retrieves the total CPU time (user + system) consumed by a specific process by reading the `/proc/[pid]/stat` file.

#### Parameters
- `int pid`: The Process ID of the target process.

#### Returns
- `long`: The total CPU time in jiffies, or 0 if it cannot be determined.

#### Key Details
- Parses the process-specific stat file to extract the relevant fields.
- Ensures error handling for cases where the stat file is inaccessible or invalid.

---

### 6. `double calculateCpuUsage(long processTimeDelta, long totalCpuTimeDelta, long numCores)`
#### Description
Calculates the CPU usage percentage for a process based on the difference in process time and total CPU time between two intervals.

#### Parameters
- `long processTimeDelta`: The change in CPU time for the process.
- `long totalCpuTimeDelta`: The change in total system CPU time.
- `long numCores`: The number of CPU cores available.

#### Returns
- `double`: The calculated CPU usage percentage.

#### Key Details
- Normalizes the process time delta against the total CPU time delta and adjusts for the number of cores.
- Handles edge cases such as zero or negative time deltas to avoid invalid results.

---

## Integration Points

- **Process Information Module**: Supplies process-level data, including total CPU and memory usage.
- **Main Module**: Invokes monitoring functions to provide real-time data for user commands such as `monitor`.
- **Process Display Module**: Uses updated process information for accurate resource usage visualization.

---

## Error Handling

- **File Access Issues**: Handles inaccessible or corrupted files gracefully by skipping affected processes.
- **Performance**: Optimized to minimize system overhead during continuous monitoring.
- **Data Accuracy**: Ensures consistency between monitored metrics by synchronizing updates.

---

# Command Handler Module Documentation

## Overview

The **Command Handler Module** facilitates user interaction with the Process Manager application. It manages the command-line interface (CLI), processes user inputs, and executes the appropriate actions. This module also supports features like auto-completion, signal handling, and help documentation, enhancing user experience and application usability.

---

## Constants and Variables

### 1. **Text Formatting Constants**
#### Description
Provides ANSI escape codes for formatting text output in the CLI, enabling better readability.
- `RESET`: Resets text formatting.
- `CYAN`, `YELLOW`, `GREEN`, `RED`: Colors for differentiating text.
- `BOLD`: Makes text bold.

### 2. **`std::thread monitoringThread`**
#### Description
A global thread used for running the process monitoring logic. Controlled by the command loop.

### 3. **`const std::vector<std::string> commands`**
#### Description
A list of supported commands for the application. Used for auto-completion and validation.

---

## Functions

### 1. `char* commandGenerator(const char* text, int state)`
#### Description
Generates command suggestions for auto-completion.

#### Parameters
- `text`: The current user input.
- `state`: The state of the completion process (0 for a new sequence).

#### Returns
- A pointer to the next matching command, or `nullptr` if no more matches are found.

#### Key Details
- Integrates with the Readline library for interactive command-line input.
- Matches user input with available commands from the `commands` list.

---

### 2. `char** commandCompleter(const char* text, int start, int end)`
#### Description
Provides auto-completion support for the CLI by returning a list of matching commands.

#### Parameters
- `text`: The current user input.
- `start`, `end`: The positions in the input string where completion is applied.

#### Returns
- A list of possible completions, or `nullptr` if no matches are found.

#### Key Details
- Enhances usability by reducing typing effort and minimizing errors.

---

### 3. `void handleSigint(int sig)`
#### Description
Handles `SIGINT` signals (e.g., Ctrl+C), ensuring graceful termination of monitoring threads and maintaining application responsiveness.

#### Parameters
- `sig`: The signal number (typically `SIGINT`).

#### Key Details
- Stops the active monitoring thread to prevent resource leaks.
- Ensures that the application remains operational after receiving the signal.

---

### 4. `void printHelp()`
#### Description
Displays a help menu to the user, detailing all available commands and their usage.

#### Key Details
- Lists commands along with brief descriptions of their functionality.
- Provides a quick reference for users unfamiliar with the application.

#### Example Output
```
Available Commands:
- start_monitor: Begin monitoring processes.
- stop_monitor: Stop the active monitoring process.
- list: Display a list of active processes.
- terminate <PID>: Terminate a process by its PID.
- help: Display this help menu.
```

---

### 5. `void startCommandLoop()`
#### Description
Starts the interactive command loop where users can input commands to control the Process Manager.

#### Key Details
- Continuously reads user input and processes commands.
- Validates commands against the `commands` list and executes corresponding functions.
- Integrates with the Readline library for enhanced CLI functionality, including history and auto-completion.

---

## Integration Points

- **Resource Monitoring Module**: Controls the monitoring thread, starting and stopping it based on user input.
- **Process Display Module**: Utilizes its functions to display process information in response to commands.
- **Main Module**: Serves as the entry point to initialize and start the command handler.

---

## Error Handling

- **Invalid Commands**: Provides meaningful error messages and suggestions for invalid inputs.
- **Signal Safety**: Ensures application stability during interruptions like `SIGINT`.
- **Thread Management**: Safely manages the lifecycle of the monitoring thread, avoiding resource leaks or crashes.
