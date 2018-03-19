//
// Created by James Barker on 16/11/2016.
//

#ifndef LITTLEPNES_MEMORY_H
#define LITTLEPNES_MEMORY_H


#include <cstdint>
#include "System.h"

class IMemory {
public:
    virtual void Write(uint16_t addr, uint8_t value) = 0;
    virtual void WriteZP(uint8_t addr, uint8_t value) = 0;
    virtual void WriteRange(uint16_t start, uint16_t end, uint8_t value) = 0;
    virtual uint8_t Read(uint16_t addr) = 0;
    virtual uint8_t ReadZP(uint8_t addr) = 0;
    virtual bool PageIfRequired(uint16_t addr1, uint16_t addr2) = 0;
    virtual void ForcedPage(uint16_t addr1, uint16_t addr2) = 0;

    virtual void Reset() = 0;
    IMemory(ISystem* system);

protected:
    ISystem* _system;
};

class CpuMemory : public IMemory {
private:

    uint8_t _ram[0x0800];
    uint8_t _ppuregs[0x08];
    uint8_t _nesapu[0x18];
    uint8_t _apu[0x08];


    //TODO: rest of hardware address bus
    uint8_t _other[0xFFFF];
public:
    void Write(uint16_t addr, uint8_t value);
    void WriteZP(uint8_t addr, uint8_t value);
    void WriteRange(uint16_t start, uint16_t end, uint8_t value);
    uint8_t Read(uint16_t addr);
    uint8_t ReadZP(uint8_t addr);
    bool PageIfRequired(uint16_t addr1, uint16_t addr2);
    void ForcedPage(uint16_t addr1, uint16_t addr2);

    void Reset();

    CpuMemory(ISystem *system);
};


#endif //LITTLEPNES_MEMORY_H
