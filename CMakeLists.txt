# Especificamos la versión mínima de CMake requerida
cmake_minimum_required(VERSION 3.10)

# Nombre de tu proyecto
project(ProcessManagerProject)

# Definir el estándar de C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Definir los directorios de los archivos de cabecera
include_directories(${CMAKE_SOURCE_DIR}/include)

# Incluir todos los archivos fuente de la carpeta src
file(GLOB SOURCES "src/*.cpp")

# Crear el ejecutable a partir de los archivos fuente
add_executable(process_manager_project ${SOURCES})

# Si es necesario, puedes agregar librerías adicionales, por ejemplo:
# target_link_libraries(process_manager_project <nombre_de_libreria>)
