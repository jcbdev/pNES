//
// Created by James Barker on 16/11/2016.
//

#include "Memory.h"

CpuMemory::CpuMemory(Cart *cart) {
    _cart = cart;
    Write(0x4017, 0x00);
    Write(0x4015, 0x00);
    WriteRange(0x4000, 0x400F, 0x00);
    WriteRange(0x0000, 0x07FF, 0X00);
}

void CpuMemory::Reset() {
    Write(0x4017, 0x00);
    Write(0x4015, 0x00);
    WriteRange(0x4000, 0x400F, 0x00);
    WriteRange(0x0000, 0x07FF, 0X00);
}

uint8_t CpuMemory::Read(uint16_t addr) {
    if (addr < 0x2000) return _ram[(uint16_t)(addr % 0x0800)];
    if (addr < 0x4000) return _ppuregs[(uint16_t)((addr-0x2000) % 0x08)];
    if (addr < 0x4018) return _nesapu[(uint16_t)(addr-0x4000)];
    if (addr < 0x4020) return _apu[(uint16_t)(addr-0x4018)];
    return _cart->PrgRead(addr);
}

uint8_t CpuMemory::ReadZP(uint8_t addr) {
    if (addr < 0x2000) return _ram[(uint16_t)(addr % 0x0800)];
    if (addr < 0x4000) return _ppuregs[(uint16_t)((addr-0x2000) % 0x08)];
    if (addr < 0x4018) return _nesapu[(uint16_t)(addr-0x4000)];
    if (addr < 0x4020) return _apu[(uint16_t)(addr-0x4018)];
    return _cart->PrgRead(addr);
}

void CpuMemory::Write(uint16_t addr, uint8_t value) {
    if (addr < 0x2000) _ram[(uint16_t)(addr % 0x0800)] = value;
    if (addr < 0x4000) _ppuregs[(uint16_t)((addr-0x2000) % 0x08)] = value;
    if (addr < 0x4018) _nesapu[(uint16_t)(addr-0x4000)] = value;
    if (addr < 0x4020) _apu[(uint16_t)(addr-0x4018)] = value;
    _cart->PrgWrite(addr, value);
}

void CpuMemory::WriteZP(uint8_t addr, uint8_t value) {
    if (addr < 0x2000) _ram[(uint16_t)(addr % 0x0800)] = value;
    if (addr < 0x4000) _ppuregs[(uint16_t)((addr-0x2000) % 0x08)] = value;
    if (addr < 0x4018) _nesapu[(uint16_t)(addr-0x4000)] = value;
    if (addr < 0x4020) _apu[(uint16_t)(addr-0x4018)] = value;
    _cart->PrgWrite(addr, value);
}

void CpuMemory::WriteRange(uint16_t start, uint16_t end, uint8_t value) {
    for (int i=start; i<end+1; i++)
        Write(i, value);
}

void CpuMemory::ForcedPage(uint16_t addr1, uint16_t addr2) {
    Read((addr1 & 0xff00) | (addr2 & 0x00ff));
}

void CpuMemory::PageIfRequired(uint16_t addr1, uint16_t addr2) {
    if((addr1 & 0xff00) != (addr2 & 0xff00)) Read((addr1 & 0xff00) | (addr2 & 0x00ff));
}
