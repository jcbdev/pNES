//
// Created by jimbo on 07/02/18.
//

#ifndef PNES_SYSTEM_H
#define PNES_SYSTEM_H

//#include "Cpu.h"
//#include "Ppu.h"
//#include "./Memory.h"
//#include "../Rom/Cart.h"
//#include "../Helpers/Logger.h"
#include <stdint.h>

class ICpu;
class IMemory;
class Cart;
class IPpu;
class ILogger;
class IDebug;
class IController;

class ISystem {
public:
    ISystem();
    virtual void Configure(ICpu* cpu, IMemory* mem, Cart* cart, IPpu* ppu, IController* controller1, IDebug *debug, ILogger* logger) = 0;
    virtual void Reset() = 0;
    virtual void Step() = 0;

    ICpu* cpu;
    IMemory* mem;
    Cart* cart;
    IPpu* ppu;
    IController* controller1;
    IDebug *debug;
    ILogger* logger;
    uint64_t cycle;
};

class System : public ISystem {
public:
    System();
    void Configure(ICpu* cpu, IMemory* mem, Cart* cart, IPpu* ppu, IController* controller1, IDebug* debug, ILogger* logger) override;
    void Reset() override;
    void Step() override;
};


#endif //PNES_SYSTEM_H
