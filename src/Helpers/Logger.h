//
// Created by James Barker on 03/10/2017.
//

#ifndef LITTLEPNES_LOGGER_H
#define LITTLEPNES_LOGGER_H

#include <string>
#include <iostream>
#include <fstream>
#include "../Core/System.h"

class ILogger {
public:
    explicit ILogger(ISystem* system);
    virtual void Log(std::string log) = 0;

protected:
    ISystem* _system;
};

class ConsoleLogger : public ILogger {
public:
    explicit ConsoleLogger(ISystem* system);
    void Log(std::string log);
};

class FileLogger : public ILogger {
private:
    std::ofstream logFile;
public:
    explicit FileLogger(ISystem* system);
    void Log(std::string log);
    ~FileLogger();
};

#endif //LITTLEPNES_LOGGER_H
