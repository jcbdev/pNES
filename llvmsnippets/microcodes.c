//
// Created by jimbo on 22/09/19.
//

#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

struct state {
    uint8_t ram[0x800];
    uint16_t pc;
    uint8_t x;
    uint8_t y;
    uint8_t a;
    uint8_t s;
    bool n, v, d, i, z, c;
    uint8_t val;
    uint16_t addr;
    int32_t clocks;
    int64_t cycle;
    bool irq;
    bool apu;
    bool nmiPending;
    bool interruptPending;

} cpuState;

void read(struct state *cpuState) {
    if (cpuState->addr < 0x2000) cpuState->val = cpuState->ram[(uint16_t)(cpuState->addr % 0x0800)];
}

void readPc(struct state *cpuState) {
    cpuState->addr = cpuState->pc;
    read(cpuState);
}

void readPcAndInc(struct state *cpuState) {
    cpuState->addr = cpuState->pc++;
    read(cpuState);
}

void testInterrupt(struct state *cpuState) {
    cpuState->interruptPending = ((cpuState->irq | cpuState->apu) & ~cpuState->i) | cpuState->nmiPending;
}

void addClocks(struct state *cpuState){
    cpuState->clocks += 3;
}

void SetFlagI(struct state *cpuState) {
    cpuState->i = true;
}

void ClearFlagD(struct state *cpuState) {
    cpuState->d = false;
}

int main() {
    cpuState.pc = 0101;
    cpuState.x = 0;
    cpuState.y = 0;
    cpuState.a = 0xFF;

    cpuState.s = 0xFD;
    cpuState.i = true;

    addClocks(&cpuState);
    readPc(&cpuState);
    read(&cpuState);
    readPcAndInc(&cpuState);
    testInterrupt(&cpuState);
    SetFlagI(&cpuState);
    ClearFlagD(&cpuState);

    return cpuState.val;
}

