# Project Tasks

## Module 1: Process Management (Process Manager)
### Submodules and Tasks:

#### Retrieve the List of Processes
- Research how to access the list of processes in your operating system.
- Implement the function that obtains the IDs of active processes.
- Implement the function that retrieves the name of each process.

#### Retrieve CPU and Memory Usage of Processes
- Research how to obtain CPU and memory usage information for processes (you can use system tools like `ps` or files in `/proc` on Linux).
- Create a function to retrieve CPU and memory usage for a process given its PID.

#### Display Processes
- Create a function to display the list of processes in the console with their PID, name, CPU, and memory usage.

#### Performance Optimization
- Conduct tests to see if retrieving processes is efficient under load (e.g., with many processes).
- Implement a process caching strategy if necessary to improve performance.

## Module 2: Parallelized Resource Monitoring
### Submodules and Tasks:

#### Collect Real-Time Resource Data (CPU and Memory)
- Create a thread that retrieves CPU usage for all processes.
- Create a thread that retrieves memory usage for all processes.
- Conduct performance tests to ensure that threads do not cause blocking issues.

#### Implement a Thread Pool
- Research how to implement an efficient thread pool (you can use a library like `std::thread` or `std::async` in C++).
- Create a function to manage multiple threads responsible for monitoring processes.

#### Optimize Thread Usage
- Avoid creating unnecessary or inefficient threads, ensuring that the number of threads is not too high to consume excessive CPU.
- Limit the frequency of monitoring updates, e.g., updating every X seconds.

#### Thread Result Management and Synchronization
- Ensure that CPU and memory usage data remains synchronized between threads (you can use mutexes or `std::atomic`).
- Test for correct synchronization and consistency of thread results.

## Module 3: Process Control
### Submodules and Tasks:

#### Implement Process Termination Function (kill)
- Research how to send termination signals to processes in your system (e.g., using `kill` in Linux or specific APIs in other OS).
- Implement the function to kill a process given its PID.

#### Verify Termination Permissions
- Ensure that only processes that the user has access to (and appropriate permissions) can be terminated.
- Handle exceptions or errors if a user attempts to terminate a process they do not have access to.

#### User Interaction
- Implement command-line functionality to allow the user to input the PID of a process to be terminated.
- Add a confirmation mechanism before terminating the process (to avoid mistakes).

#### Error and Exception Handling
- Capture and handle any errors that may occur during the termination of a process (e.g., when the process no longer exists).
- Ensure that the system notifies the user of errors or failures.

## Module 4: Logging and Error Handling
### Submodules and Tasks:

#### Implement Logging System
- Create a class or function that writes logs to a file with timestamps.
- Ensure that logs contain relevant information such as user actions (process termination, etc.) and any errors that occur.

#### Log User Actions
- Log each user action, such as process termination or resource updates.
- Ensure that the logging system is structured for easy reading and debugging.

#### Handle Critical Errors
- Log any critical errors that occur during program execution.
- Implement an error notification system for critical errors (e.g., messages in the console or a log file).

#### Logging Performance Optimization
- Ensure that the logging system does not negatively impact program performance.
- Consider implementing asynchronous logging if necessary.

## Module 5: Optimization (HPC Optimization)
### Submodules and Tasks:

#### Identify Causes of CPU or Memory Overload
- Analyze resource usage to identify possible causes of overload (e.g., too many threads, high update frequency, etc.).
- Use profiling tools to obtain performance statistics.

#### Reduce Data Update Frequency
- Implement a way to reduce the frequency of resource data updates, e.g., updating every 1-2 seconds instead of every
