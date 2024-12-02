# Tareas del Proyecto

## Módulo 1: Gestión de procesos (Process Manager)
### Submódulos y tareas:

#### Obtener la lista de procesos
- Investigar cómo acceder a la lista de procesos en tu sistema operativo.
- Implementar la función que obtiene los IDs de los procesos activos.
- Implementar la función que obtiene el nombre de cada proceso.

#### Obtener el uso de CPU y memoria de los procesos
- Investigar cómo obtener la información de uso de CPU y memoria de los procesos (puedes usar herramientas del sistema como ps o archivos en /proc en Linux).
- Crear una función para recuperar el uso de CPU y memoria de un proceso dado su PID.

#### Mostrar los procesos
- Crear una función para mostrar en consola la lista de procesos con su PID, nombre, CPU y memoria.

#### Optimización del rendimiento
- Realizar pruebas para ver si la obtención de procesos es eficiente bajo carga (por ejemplo, con muchos procesos).
- Implementar una estrategia de cacheo de procesos si es necesario para mejorar el rendimiento.

## Módulo 2: Monitoreo paralelo de recursos (Parallelized Resource Monitoring)
### Submódulos y tareas:

#### Recopilar datos de recursos (CPU y memoria) en tiempo real
- Crear un hilo que recupere el uso de CPU de todos los procesos.
- Crear un hilo que recupere el uso de memoria de todos los procesos.
- Realizar pruebas de rendimiento para garantizar que los hilos no causen problemas de bloqueo.

#### Implementar un pool de hilos
- Investigar cómo implementar un pool de hilos eficiente (puedes usar una librería como std::thread o std::async en C++).
- Crear una función para gestionar múltiples hilos que se encargan del monitoreo de los procesos.

#### Optimizar el uso de hilos
- Evitar la creación de hilos innecesarios o ineficientes, asegurando que el número de hilos no sea tan alto que consuma demasiado CPU.
- Limitar la frecuencia de actualizaciones de los datos de monitoreo, por ejemplo, realizando actualizaciones cada X segundos.

#### Manejo de resultados y sincronización de hilos
- Asegurarse de que los datos de uso de CPU y memoria se mantengan sincronizados entre los hilos (puedes usar mutexes o std::atomic).
- Probar la correcta sincronización y la coherencia de los resultados de los hilos.

## Módulo 3: Control de procesos (Process Control)
### Submódulos y tareas:

#### Implementar la función de terminación de procesos (kill)
- Investigar cómo enviar señales de terminación a los procesos en tu sistema (por ejemplo, usando kill en Linux o API específicas en otros SO).
- Implementar la función para matar un proceso dado su PID.

#### Verificar permisos de terminación
- Asegurarse de que solo los procesos a los que el usuario tiene acceso (y permisos adecuados) puedan ser terminados.
- Gestionar las excepciones o errores que se produzcan si un usuario intenta terminar un proceso al que no tiene acceso.

#### Interacción con el usuario
- Implementar la funcionalidad de la línea de comandos para permitir al usuario ingresar el PID de un proceso a terminar.
- Agregar un mecanismo de confirmación antes de terminar el proceso (para evitar errores).

#### Manejo de errores y excepciones
- Capturar y manejar cualquier error que pueda ocurrir durante la terminación de un proceso (por ejemplo, cuando el proceso ya no existe).
- Asegurarse de que el sistema notifique al usuario sobre errores o fallos.

## Módulo 4: Registro y manejo de errores (Logging and Error Handling)
### Submódulos y tareas:

#### Implementar el sistema de logs
- Crear una clase o función que permita escribir logs en un archivo con timestamps.
- Asegurarse de que los logs contengan información relevante como las acciones del usuario (terminación de procesos, etc.) y cualquier error que ocurra.

#### Registrar las acciones del usuario
- Registrar cada acción del usuario, como la terminación de un proceso o la actualización de recursos.
- Asegurarse de que el sistema de logs esté estructurado para que sea fácil de leer y depurar.

#### Manejo de errores críticos
- Registrar cualquier error crítico que ocurra durante la ejecución del programa.
- Implementar un sistema de notificación de errores críticos al usuario (por ejemplo, mensajes en la consola o en un archivo de log).

#### Optimización del rendimiento del logging
- Asegurarse de que el sistema de logs no afecte negativamente el rendimiento del programa.
- Considerar la implementación de logs asincrónicos si es necesario.

## Módulo 5: Optimización (HPC Optimization)
### Submódulos y tareas:

#### Identificación de causas de sobrecarga de CPU o memoria
- Realizar un análisis de uso de recursos para identificar las posibles causas de sobrecarga (por ejemplo, demasiados hilos, frecuencia alta de actualización, etc.).
- Utilizar herramientas de profiling para obtener estadísticas de rendimiento.

#### Reducir la frecuencia de actualización de datos
- Implementar una forma de reducir la frecuencia con la que se actualizan los datos de recursos, por ejemplo, actualizando cada 1-2 segundos en lugar de cada 0.1 segundos.

#### Optimizar el uso de hilos
- Reducir el número de hilos activos cuando no sea necesario.
- Implementar un pool de hilos para evitar la creación de hilos innecesarios y controlar el número de hilos activos.

#### Uso de estructuras de datos ligeras
- Evaluar si las estructuras de datos que estás utilizando son demasiado pesadas para la carga del sistema.
- Implementar estructuras de datos más ligeras y optimizadas para la cantidad de información que estás manejando.
