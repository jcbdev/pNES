//
// Created by James Barker on 07/10/2017.
//

#include "Nrom.h"

uint8_t Nrom::PrgRead(uint16_t addr) {
    if (addr >= 0xC000) return prgData[UpperPrgBank][addr - 0xC000];
    else if (addr > 0x8000) return prgData[LowerPrgBank][addr - 0x8000];
    else return 0;
}

uint8_t Nrom::PrgWrite(uint16_t addr, uint8_t data) {}

uint8_t Nrom::ChrRead(uint16_t addr) {
    return chrData[]
}