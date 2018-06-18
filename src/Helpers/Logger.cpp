//
// Created by James Barker on 03/10/2017.
//

#include "Logger.h"

ILogger::ILogger(ISystem *system) {
    _system = system;
}

ConsoleLogger::ConsoleLogger(ISystem* system) : ILogger(system){

}

void ConsoleLogger::Log(std::string log) {
    std::cout << log << std::endl;
}

void FileLogger::Log(std::string log) {
    //if (_system->totalClocks < 0xe4000)
        return;
    logFile << log << std::endl;
    logFile.flush();
}

FileLogger::FileLogger(ISystem* system) : ILogger(system) {
    logFile.open("trace.log", std::ios::out | std::ios::trunc);
    if (!logFile.is_open()) {
        std::cout << "Error opening log file" << std::endl;
    }
}

FileLogger::~FileLogger() {
    logFile.close();
}
