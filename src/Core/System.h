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

class ISystem {
public:
    ISystem();
    virtual void Configure(ICpu* cpu, IMemory* mem, Cart* cart, IPpu* ppu, ILogger* logger) = 0;
    virtual void Reset() = 0;

    ICpu* cpu;
    IMemory* mem;
    Cart* cart;
    IPpu* ppu;
    ILogger* logger;
    uint32_t totalClocks;
};

class System : public ISystem {
public:
    System();
    void Configure(ICpu* cpu, IMemory* mem, Cart* cart, IPpu* ppu, ILogger* logger) override;
    void Reset() override;
};


#endif //PNES_SYSTEM_H
