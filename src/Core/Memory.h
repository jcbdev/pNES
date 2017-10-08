//
// Created by James Barker on 16/11/2016.
//

#ifndef LITTLEPNES_MEMORY_H
#define LITTLEPNES_MEMORY_H


#include <cstdint>
#include "../Rom/Cart.h"

class IMemory {
public:
    virtual void Write(uint16_t addr, uint8_t value) = 0;
    virtual void WriteZP(uint8_t addr, uint8_t value) = 0;
    virtual void WriteRange(uint16_t start, uint16_t end, uint8_t value) = 0;
    virtual uint8_t Read(uint16_t addr) = 0;
    virtual uint8_t ReadZP(uint8_t addr) = 0;
    virtual void PageIfRequired(uint16_t addr1, uint16_t addr2) = 0;
    virtual void ForcedPage(uint16_t addr1, uint16_t addr2) = 0;

    virtual void Reset() = 0;
    IMemory() {};
};

class Memory : public IMemory {
private:

    uint8_t _ram[0x0800];
    Cart* _cart;

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

    Memory(Cart *cart);
};


#endif //LITTLEPNES_MEMORY_H
