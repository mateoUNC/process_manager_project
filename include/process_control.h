#ifndef PROCESS_CONTROL_H
#define PROCESS_CONTROL_H

#include <string> 

bool killProcess(int pid);
bool killProcessesByCpu(double threshold);
bool killProcessesByUser(const std::string& username);

#endif // PROCESS_CONTROL_H
