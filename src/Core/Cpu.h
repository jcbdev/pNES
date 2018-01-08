//
// Created by James Barker on 16/11/2016.
//

#ifndef LITTLEPNES_CPU_H
#define LITTLEPNES_CPU_H


#include <memory>
#include "Memory.h"
#include "Core.h"
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

class ICpu {
public:
    ICpu(IMemory* memory, ILogger* logger);

    uint8_t x;
    uint8_t y;
    uint8_t s;
    uint8_t a;
    uint16_t pc;
    CpuFlags p;
    uint32_t ticks;

    virtual void Reset() = 0;
    virtual void Cycle() = 0;
    virtual bool Interrupt() = 0;
    virtual void Apu(bool line) = 0;
    virtual void Irq(bool line) = 0;
    virtual void Nmi(bool line) = 0;

    bool error;

protected:
    IMemory* _mem;
    ILogger* _logger;
};

class Cpu : public ICpu {
public:
    Cpu(IMemory*  memory, ILogger* logger);

    virtual void Reset();
    virtual void Cycle();
    virtual bool Interrupt();
    virtual void Apu(bool line);
    virtual void Irq(bool line);
    virtual void Nmi(bool line);

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
    void Dex();
    void Dey();
    void Eor();
    void FlagClear(bool &flag);
    void FlagSet(bool &flag);
    void Inc();
    void Inx();
    void Iny();
    void JmpAbsolute();
    void JmpIndirect();
    void Jsr();
    void Lda();
    void Ldx();
    void Ldy();
    void Lsr();
    void LsrAccumulator();
    void Nop();
    void Ora();
    void Pha();
    void Php();
    void Pla();
    void Plp();
    void Rol();
    void RolAccumulator();
    void Ror();
    void RorAccumulator();
    void Rti();
    void Rts();
    void Sbc();
    void Sta();
    void Stx();
    void Sty();
    void Tax();
    void Txa();
    void Tay();
    void Tya();
    void Txs();
    void Tsx();

    //Illegals
    void IllegalNopZeroPage();
    void IllegalNopAbsolute();
    void IllegalNopZeroPageX();
    void IllegalNopImplied();
    void IllegalNopAbsoluteX();
    void IllegalArrImmediate();
    void IllegalNopImmediate();


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
    void _testInterrupt();
    bool _interrupt_pending;
    bool _nmi;
    bool _nmi_pending;
    bool _apu;
    bool _irq;
};


#endif //LITTLEPNES_CPU_H
