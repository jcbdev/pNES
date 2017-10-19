//
// Created by James Barker on 14/10/2017.
//

#ifndef LITTLEPNES_LOGGERSTUB_H
#define LITTLEPNES_LOGGERSTUB_H

#include "../../src/Helpers/Logger.h"

class LoggerStub : public ILogger {
    void Log(std::string log);
};


#endif //LITTLEPNES_LOGGERSTUB_H
