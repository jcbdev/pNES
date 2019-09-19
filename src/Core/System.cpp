//
// Created by jimbo on 07/02/18.
//

#include "System.h"
#include "Memory.h"
#include "Cpu.h"
#include "Ppu.h"
#include "Debug.h"

void System::Configure(ICpu *cpu, IMemory *mem, Cart *cart, IPpu *ppu, IController* controller1, IDebug *debug, ILogger *logger) {
    this->cpu = cpu;
    this->mem = mem;
    this->cart = cart;
    this->ppu = ppu;
    this->controller1 = controller1;
    this->debug = debug;
    this->logger = logger;
}

System::System() : ISystem::ISystem() {
}

void System::Reset() {
    mem->Reset();
    cpu->Reset();
    ppu->Reset();
    cycle = 7;
}

ISystem::ISystem() = default;

void System::Step() {
    if (debug->pause) debug->cursorPosition = cpu->pc;

    if (!debug->pause) {

        if (!cpu->Interrupt())
            cpu->Cycle();

        while (cpu->clocks > 0) {
            for (int i = 0; i < 3; i++) {
                ppu->Step();
                cpu->clocks--;
            }
        }
        debug->Refresh();
    }

    cpu->clocks = 0;
    ppu->clocks = 0;
}
