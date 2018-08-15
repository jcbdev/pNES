//
// Created by James Barker on 07/10/2017.
//

#include "Nrom.h"
#include "../Helpers/Logger.h"

Nrom::Nrom(ISystem *system) : Cart(system) {

}

Nrom::~Nrom() {

}

uint8_t Nrom::Read(uint16_t addr) {
    if (addr < 0x2000) {
        uint8_t val = Chr[ChrBank][addr];
        _system->logger->Log("        **** Read ChrRom[" + std::to_string(ChrBank) + "]: " + _tohex(addr) + " -> " + _tohex(val));
        return val;
    }
    if (addr >= 0xC000)
        return Prg[UpperPrgBank][addr-0xC000];
    if (addr >= 0x8000)
        return Prg[LowerPrgBank][addr-0x8000];
    if (addr >= 0x6000)
        return Sram[addr-0x6000];
    _system->logger->Log("Nrom Read: Invalid Cartridge address");
    return 0;
}

void Nrom::Write(uint16_t addr, uint8_t data) {
    if (addr < 0x2000) {
        _system->logger->Log("        **** Write ChrRom[" + std::to_string(ChrBank) + "]: " + _tohex(addr) + " <- " + _tohex(data));
        //Chr[ChrBank][addr] = data;
    }
    else if (addr >= 0x8000)
        UpperPrgBank = data % PrgSize;
    else if (addr >= 0x6000)
        Sram[addr-0x6000] = data;
    else _system->logger->Log("NRom Write: Invalid Cartridge address");
}

