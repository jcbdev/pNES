//
// Created by James Barker on 07/10/2017.
//

#include "Nrom.h"

Nrom::Nrom(ILogger *logger) : Cart(logger) {

}

Nrom::~Nrom() {

}

uint8_t Nrom::PrgRead(uint16_t addr) {
//    if (addr >= 0xC000) return prgData[UpperPrgBank][(uint16_t)(addr - 0xC000)];
//    else if (addr > 0x8000) return prgData[LowerPrgBank][(uint16_t)(addr - 0x8000)];
    if (addr >= 0xC000) return romData[(uint32_t)(16 + (UpperPrgBank * 0x4000) + (addr - 0xC000))];
    else if (addr > 0x8000) return romData[(uint32_t)(16 + (LowerPrgBank * 0x4000) + (addr - 0x8000))];
    else return 0;
}

void Nrom::PrgWrite(uint16_t addr, uint8_t data) {}

uint8_t Nrom::ChrRead(uint16_t addr) {
    return 0;
}

void Nrom::ChrWrite(uint16_t addr, uint8_t data) {}

