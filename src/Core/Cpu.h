//
// Created by James Barker on 16/11/2016.
//

#ifndef LITTLEPNES_CPU_H
#define LITTLEPNES_CPU_H


#include <memory>
#include "Memory.h"
#include "Core.h"

class ICpu {
public:
    ICpu(std::shared_ptr<IMemory>&  memory);

    uint8_t x;
    uint8_t y;
    uint8_t s;
    uint8_t a;
    uint16_t pc;
    CpuFlags p;

    virtual void Reset() = 0;
    virtual void Cycle() = 0;

    bool error;

protected:
    std::shared_ptr<IMemory> _mem;
};

class Cpu : public ICpu {
public:
    Cpu(std::shared_ptr<IMemory>&  memory);

    virtual void Reset();
    virtual void Cycle();

    void Read(void (Cpu::*operation)(void (Cpu::*opcode)(), bool, bool), void (Cpu::*opcode)());
    void Store(void (Cpu::*operation)(void (Cpu::*opcode)(), bool, bool), void (Cpu::*opcode)());
    void Rmw(void (Cpu::*operation)(void (Cpu::*opcode)(), bool, bool), void (Cpu::*opcode)());

    void Adc();
    void And();
    void Asl();
    void AslAccumulator();
    void Bit();
    void Branch(bool condition);
    void Brk();
    void Cmp();
    void Cpx();
    void Cpy();
    void Dec();
    void Eor();
    void FlagClear(bool &flag);
    void FlagSet(bool &flag);
    void Inc();
    void JmpAbsolute();
    void JmpIndirect();
    void Ldx();

    void Immediate(void (Cpu::*opcode)(), bool rmw, bool write);
    void Zeropage(void (Cpu::*opcode)(), bool rmw, bool write);
    void ZeropageX(void (Cpu::*opcode)(), bool rmw, bool write);
    void ZeropageY(void (Cpu::*opcode)(), bool rmw, bool write);
    void Absolute(void (Cpu::*opcode)(), bool rmw, bool write);
    void AbsoluteX(void (Cpu::*opcode)(), bool rmw, bool write);
    void AbsoluteY(void (Cpu::*opcode)(), bool rmw, bool write);
    void IndirectX(void (Cpu::*opcode)(), bool rmw, bool write);
    void IndirectY(void (Cpu::*opcode)(), bool rmw, bool write);



private:
    uint8_t _val;
    Address16 _addr16;
    uint8_t _readPcAndInc();
    uint8_t _readPc();
    void _writeSp(uint8_t data);
    uint8_t _readSp();
};


#endif //LITTLEPNES_CPU_H
