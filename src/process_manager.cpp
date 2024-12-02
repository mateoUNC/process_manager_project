#include "process_manager.h"
#include <iostream>
#include <fstream>
#include <dirent.h>
#include <cstring>

std::vector<Process> getActiveProcesses() {
    std::vector<Process> processes;
    DIR* dir = opendir("/proc"); // Abrir el directorio /proc
    
    if (dir == nullptr) {
        std::cerr << "No se pudo abrir /proc" << std::endl;
        return processes;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        // Verificar si el nombre del directorio es un número (PID)
        if (isdigit(entry->d_name[0])) {
            int pid = std::stoi(entry->d_name);
            std::string name = getProcessName(pid);

            // Agregar el proceso a la lista
            processes.push_back({pid, name});
        }
    }
    closedir(dir); // Cerrar el directorio /proc
    return processes;
}

std::string getProcessName(int pid) {
    std::string procFilePath = "/proc/" + std::to_string(pid) + "/comm";
    std::ifstream procFile(procFilePath);
    
    if (procFile.is_open()) {
        std::string processName;
        std::getline(procFile, processName); // Leer el nombre del proceso
        procFile.close();
        return processName;
    } else {
        return "Unknown"; // Si no se puede leer el nombre, devolver "Unknown"
    }
}
