//
// Created by James Barker on 08/10/2017.
//

#ifndef LITTLEPNES_MEMORYSTUB_H
#define LITTLEPNES_MEMORYSTUB_H

#include "../../src/Core/Memory.h"

class MemoryStub : public IMemory {
private:
    uint8_t _ram[0x0800];

    //TODO: rest of hardware address bus
    uint8_t _other[0xFFFF];
public:
    void Write(uint16_t addr, uint8_t value);
    void WriteZP(uint8_t addr, uint8_t value);
    void WriteRange(uint16_t start, uint16_t end, uint8_t value);
    uint8_t Read(uint16_t addr);
    uint8_t ReadZP(uint8_t addr);
    void PageIfRequired(uint16_t addr1, uint16_t addr2);
    void ForcedPage(uint16_t addr1, uint16_t addr2);

    void Reset();

    MemoryStub(ISystem* system);
};


#endif //LITTLEPNES_MEMORYSTUB_H
