//
// Created by James Barker on 16/11/2016.
//

#include "Memory.h"
#include "../Rom/Cart.h"
#include "Ppu.h"
#include "../Helpers/Logger.h"

IMemory::IMemory(ISystem *system) {
    _system = system;
}

CpuMemory::CpuMemory(ISystem *system) : IMemory::IMemory(system) {

}

void CpuMemory::Reset() {
    Write(0x4017, 0x00);
    Write(0x4015, 0x00);
    WriteRange(0x4000, 0x400F, 0x00);
    WriteRange(0x0000, 0x07FF, 0X00);
}

uint8_t CpuMemory::Read(uint16_t addr) {
    if (addr < 0x2000) return _ram[(uint16_t)(addr % 0x0800)];
    if (addr < 0x4000) return _system->ppu->ReadRegister(0x2000 + addr%8);
    if (addr == 0x4014) return _system->ppu->ReadRegister(addr);
    if (addr == 0x4015) return 0; //APU
    if (addr == 0x4016) return 0; //Controller1
    if (addr == 0x4017) return 0; //Controller2
    if (addr < 0x6000) return 0; //IORegs
    if (addr >= 0x6000) return _system->cart->Read(addr);
    _system->logger->Log("CPU Memory: Unknown memory addr");
    return 0;
}

uint8_t CpuMemory::ReadZP(uint8_t addr) {
    return Read(addr);
}

void CpuMemory::Write(uint16_t addr, uint8_t value) {
    if (addr < 0x2000) _ram[(uint16_t)(addr % 0x0800)] = value;
    else if (addr < 0x4000) _system->ppu->WriteRegister(0x2000+ (addr%8), value);
    else if (addr < 0x4014) {} //APU
    else if (addr == 0x4014) _system->ppu->WriteRegister(addr, value);
    else if (addr == 0x4015) {} //APU
    else if (addr == 0x4016) {} //Controller1,2
    else if (addr == 0x4017) {} //APU
    else if (addr < 0x6000) {} //IORegs
    else _system->cart->Write(addr, value);
}

void CpuMemory::WriteZP(uint8_t addr, uint8_t value) {
    Write(addr, value);
}

void CpuMemory::WriteRange(uint16_t start, uint16_t end, uint8_t value) {
    for (int i=start; i<end+1; i++)
        Write(i, value);
}

void CpuMemory::ForcedPage(uint16_t addr1, uint16_t addr2) {
    Read((addr1 & 0xff00) | (addr2 & 0x00ff));
}

bool CpuMemory::PageIfRequired(uint16_t addr1, uint16_t addr2) {
    if((addr1 & 0xff00) != (addr2 & 0xff00)) {
        Read((addr1 & 0xff00) | (addr2 & 0x00ff));
        return true;
    }
    return false;
}
