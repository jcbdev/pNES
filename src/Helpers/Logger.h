//
// Created by James Barker on 03/10/2017.
//

#ifndef LITTLEPNES_LOGGER_H
#define LITTLEPNES_LOGGER_H

#include <string>
#include <iostream>

class Logger {
public:
    virtual void Log(std::string log) = 0;
};

class ConsoleLogger : public Logger {
public:
    virtual void Log(std::string log);
};


#endif //LITTLEPNES_LOGGER_H
