//
// Created by jimbo on 07/02/18.
//

#include "System.h"
#include "Memory.h"
#include "Cpu.h"
#include "Ppu.h"

void System::Configure(ICpu *cpu, IMemory *mem, Cart *cart, IPpu *ppu, IDebug *debug, ILogger *logger) {
    this->cpu = cpu;
    this->mem = mem;
    this->cart = cart;
    this->ppu = ppu;
    this->debug = debug;
    this->logger = logger;
}

System::System() : ISystem::ISystem() {
}

void System::Reset() {
    mem->Reset();
    cpu->Reset();
    ppu->Reset();
    totalClocks = 7;
}

ISystem::ISystem() = default;
