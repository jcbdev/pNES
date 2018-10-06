//
// Created by jimbo on 24/09/18.
//

#include "Apu.h"

IApu::IApu(ISystem *system) {
    _system = system;
}

Apu::Apu(ISystem *system) : IApu(system) {
    for (int i=0; i < 31; i++) {
        _pulseTable[i] = 95.52f / (8128.0f/(float)(i) + 100);
    }
    for (int i=0; i < 203; i++) {
        _tndTable[i] = 163.67f / (24329.0f/(float)(i) + 100);
    }
}

uint8_t Apu::Read(uint16_t addr) {
    return 0;
}

void Apu::Write(uint16_t addr, uint8_t data) {

}
