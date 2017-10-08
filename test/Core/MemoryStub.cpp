//
// Created by James Barker on 08/10/2017.
//

#include "MemoryStub.h"

MemoryStub::MemoryStub() {
    Write(0x4017, 0x00);
    Write(0x4015, 0x00);
    WriteRange(0x4000, 0x400F, 0x00);
    WriteRange(0x0000, 0x07FF, 0X00);
}

void MemoryStub::Reset() {
    Write(0x4017, 0x00);
    Write(0x4015, 0x00);
    WriteRange(0x4000, 0x400F, 0x00);
    WriteRange(0x0000, 0x07FF, 0X00);
}

uint8_t MemoryStub::Read(uint16_t addr) {
    return _other[addr];
}

uint8_t MemoryStub::ReadZP(uint8_t addr) {
    return _other[addr];
}

void MemoryStub::Write(uint16_t addr, uint8_t value) {
    _other[addr] = value;
}

void MemoryStub::WriteZP(uint8_t addr, uint8_t value) {
    _other[addr] = value;
}

void MemoryStub::WriteRange(uint16_t start, uint16_t end, uint8_t value) {
    for (int i=start; i<end+1; i++)
        Write(i, value);
}

void MemoryStub::ForcedPage(uint16_t addr1, uint16_t addr2) {
    Read((addr1 & 0xff00) | (addr2 & 0x00ff));
}

void MemoryStub::PageIfRequired(uint16_t addr1, uint16_t addr2) {
    if((addr1 & 0xff00) != (addr2 & 0xff00)) Read((addr1 & 0xff00) | (addr2 & 0x00ff));
}
