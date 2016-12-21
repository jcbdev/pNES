//
// Created by James Barker on 16/11/2016.
//

#include "Memory.h"

Memory::Memory(){
    Write(0x4017, 0x00);
    Write(0x4015, 0x00);
    WriteRange(0x4000, 0x400F, 0x00);
    WriteRange(0x0000, 0x07FF, 0X00);
}

void Memory::Reset() {
    Write(0x4017, 0x00);
    Write(0x4015, 0x00);
    WriteRange(0x4000, 0x400F, 0x00);
    WriteRange(0x0000, 0x07FF, 0X00);
}

uint8_t Memory::Read(uint16_t addr) {
    return _other[addr];
}

uint8_t Memory::ReadZP(uint8_t addr) {
    return _other[addr];
}

void Memory::Write(uint16_t addr, uint8_t value) {
    _other[addr] = value;
}

void Memory::WriteZP(uint8_t addr, uint8_t value) {
    _other[addr] = value;
}

void Memory::WriteRange(uint16_t start, uint16_t end, uint8_t value) {
    for (int i=start; i<end+1; i++)
        Write(i, value);
}

void Memory::ForcedPage(uint16_t addr1, uint16_t addr2) {
    Read((addr1 & 0xff00) | (addr2 & 0x00ff));
}

void Memory::PageIfRequired(uint16_t addr1, uint16_t addr2) {
    if((addr1 & 0xff00) != (addr2 & 0xff00)) Read((addr1 & 0xff00) | (addr2 & 0x00ff));
}
