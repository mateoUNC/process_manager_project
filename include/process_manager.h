#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include <vector>
#include <string>

// Declaración de la estructura para representar un proceso
struct Process {
    int pid;
    std::string name;
};

// Función para obtener todos los procesos activos
std::vector<Process> getActiveProcesses();

// Función para obtener el nombre del proceso a partir de su PID
std::string getProcessName(int pid);

#endif // PROCESS_MANAGER_H
