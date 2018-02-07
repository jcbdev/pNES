//
// Created by jimbo on 07/02/18.
//

#include "System.h"
#include "Memory.h"
#include "Cpu.h"

void System::Configure(ICpu *cpu, IMemory *mem, Cart *cart, IPpu *ppu, ILogger *logger) {
    this->cpu = cpu;
    this->mem = mem;
    this->cart = cart;
    this->ppu = ppu;
    this->logger = logger;
}

System::System() : ISystem::ISystem() {
}

void System::Reset() {
    mem->Reset();
    cpu->Reset();
}

ISystem::ISystem() = default;
