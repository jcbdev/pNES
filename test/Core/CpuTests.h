//
// Created by James Barker on 27/11/2016.
//

#ifndef LITTLEPNES_CPUTESTS_H
#define LITTLEPNES_CPUTESTS_H

#include <gtest/gtest.h>
#include "../../src/Core/Memory.h"
#include "MemoryStub.h"
#include "../../src/Core/Cpu.h"

CpuFlags nvdizc(bool n = false, bool v = false, bool d = false, bool i = true, bool z = false, bool c = false) {
    CpuFlags flags;
    flags = 0x34;
    flags.c = c;
    flags.z = z;
    flags.i = i;
    flags.d = d;
    flags.v = v;
    flags.n = n;
    return flags;
}

void ExecuteImmediate(ICpu *cpu, std::shared_ptr<IMemory> mem, uint8_t opcode, uint8_t value, CpuFlags *flags, uint8_t a=0, uint8_t x=0, uint8_t y=0, uint8_t s=0xFD) {
    cpu->Reset();
    cpu->a = a;
    cpu->x = x;
    cpu->y = y;
    cpu->s = s;
    mem->Write(0x8000, opcode);
    mem->Write(0x8001, value);
    if (flags!= nullptr) cpu->p = *flags;
    cpu->Cycle();
    EXPECT_EQ(cpu->pc, 0x8002);
    EXPECT_FALSE(cpu->error);
}

void ExecuteAccumulator(ICpu *cpu, std::shared_ptr<IMemory> mem, uint8_t opcode, CpuFlags *flags, uint8_t a=0, uint8_t x=0, uint8_t y=0, uint8_t s=0xFD) {
    cpu->Reset();
    cpu->a = a;
    cpu->x = x;
    cpu->y = y;
    cpu->s = s;
    mem->Write(0x8000, opcode);
    if (flags!= nullptr) cpu->p = *flags;
    cpu->Cycle();
    EXPECT_EQ(cpu->pc, 0x8001);
    EXPECT_FALSE(cpu->error);
}

void ExecuteBrk(ICpu *cpu, std::shared_ptr<IMemory> mem, uint8_t opcode, uint8_t l, uint8_t h, CpuFlags *flags, uint8_t a=0, uint8_t x=0, uint8_t y=0, uint8_t s=0xFD) {
    cpu->Reset();
    cpu->a = a;
    cpu->x = x;
    cpu->y = y;
    cpu->s = s;
    mem->Write(0x8000, opcode);
    mem->Write(0xFFFE, l);
    mem->Write(0xFFFF, h);
    if (flags!= nullptr) cpu->p = *flags;
    auto expectedP = cpu->p | 0x30;
    cpu->Cycle();
    EXPECT_EQ(mem->Read(0x100 | ++cpu->s), expectedP);
    EXPECT_EQ(mem->Read(0x100 | ++cpu->s), 0x02);
    EXPECT_EQ(mem->Read(0x100 | ++cpu->s), 0x80);
    EXPECT_EQ(cpu->pc, l | (h<<8));
    EXPECT_FALSE(cpu->error);
}

void ExecuteBranch(ICpu *cpu, std::shared_ptr<IMemory> mem, uint8_t opcode, uint8_t value, CpuFlags *flags, bool branches, uint8_t a=0, uint8_t x=0, uint8_t y=0, uint8_t s=0xFD) {
    cpu->Reset();
    cpu->a = a;
    cpu->x = x;
    cpu->y = y;
    cpu->s = s;
    mem->Write(0x8000, opcode);
    mem->Write(0x8001, value);
    if (flags!= nullptr) cpu->p = *flags;
    cpu->Cycle();
    if (branches) EXPECT_EQ(cpu->pc, 0x8002 + (uint8_t)value);
    else EXPECT_EQ(cpu->pc, 0x8002);
    EXPECT_FALSE(cpu->error);
}

void ExecuteFlagOperation(ICpu *cpu, std::shared_ptr<IMemory> mem, uint8_t opcode, CpuFlags *flags, uint8_t a=0, uint8_t x=0, uint8_t y=0, uint8_t s=0xFD) {
    cpu->Reset();
    cpu->a = a;
    cpu->x = x;
    cpu->y = y;
    cpu->s = s;
    mem->Write(0x8000, opcode);
    if (flags!= nullptr) cpu->p = *flags;
    cpu->Cycle();
    EXPECT_EQ(cpu->pc, 0x8001);
    EXPECT_FALSE(cpu->error);
}

void ExecuteZeroPage(ICpu *cpu, std::shared_ptr<IMemory> mem, uint8_t opcode, uint8_t value, CpuFlags *flags, uint8_t a=0, uint8_t x=0, uint8_t y=0, uint8_t s=0xFD) {
    cpu->Reset();
    cpu->a = a;
    cpu->x = x;
    cpu->y = y;
    cpu->s = s;
    mem->Write(0x001F, value);
    mem->Write(0x8000, opcode);
    mem->Write(0x8001, 0x1F);
    if (flags!=nullptr) cpu->p = *flags;
    cpu->Cycle();
    EXPECT_EQ(cpu->pc, 0x8002);
    EXPECT_FALSE(cpu->error);
}

void EXPECT_ZeroPage(std::shared_ptr<IMemory> mem, uint8_t value){
    EXPECT_EQ(mem->Read(0x001F), value);
}

void ExecuteZeroPageX(ICpu *cpu, std::shared_ptr<IMemory> mem, uint8_t opcode, uint8_t value, CpuFlags *flags, uint8_t a=0, uint8_t x=0, uint8_t y=0, uint8_t s=0xFD) {
    cpu->Reset();
    cpu->a = a;
    cpu->x = x;
    cpu->y = y;
    cpu->s = s;
    mem->Write(0x0020, value);
    mem->Write(0x8000, opcode);
    mem->Write(0x8001, 0x1F);
    cpu->x = 0x01;
    if (flags!=nullptr) cpu->p = *flags;
    cpu->Cycle();
    EXPECT_EQ(cpu->pc, 0x8002);
    EXPECT_FALSE(cpu->error);
}

void EXPECT_ZeroPageX(std::shared_ptr<IMemory> mem, uint8_t value){
    EXPECT_EQ(mem->Read(0x0020), value);
}

void ExecuteZeroPageY(ICpu *cpu, std::shared_ptr<IMemory> mem, uint8_t opcode, uint8_t value, CpuFlags *flags, uint8_t a=0, uint8_t x=0, uint8_t y=0, uint8_t s=0xFD) {
    cpu->Reset();
    mem->Write(0x0020, value);
    mem->Write(0x8000, opcode);
    mem->Write(0x8001, 0x1F);
    cpu->y = 0x01;
    if (flags!=nullptr) cpu->p = *flags;
    cpu->Cycle();
    EXPECT_EQ(cpu->pc, 0x8002);
    EXPECT_FALSE(cpu->error);
}

void EXPECT_ZeroPageY(std::shared_ptr<IMemory> mem, uint8_t value){
    EXPECT_EQ(mem->Read(0x0020), value);
}

void ExecuteAbsolute(ICpu *cpu, std::shared_ptr<IMemory> mem, uint8_t opcode, uint8_t value, CpuFlags *flags, uint8_t a=0, uint8_t x=0, uint8_t y=0, uint8_t s=0xFD) {
    cpu->Reset();
    cpu->a = a;
    cpu->x = x;
    cpu->y = y;
    cpu->s = s;
    mem->Write(0x8180, value);
    mem->Write(0x8000, opcode);
    mem->Write(0x8001, 0x80);
    mem->Write(0x8002, 0x81);
    if (flags!=nullptr) cpu->p = *flags;
    cpu->Cycle();
    EXPECT_EQ(cpu->pc, 0x8003);
    EXPECT_FALSE(cpu->error);
}

void EXPECT_Absolute(std::shared_ptr<IMemory> mem, uint8_t value){
    EXPECT_EQ(mem->Read(0x8180), value);
}

void ExecuteAbsoluteX(ICpu *cpu, std::shared_ptr<IMemory> mem, uint8_t opcode, uint8_t value, CpuFlags *flags, uint8_t a=0, uint8_t x=0, uint8_t y=0, uint8_t s=0xFD) {
    cpu->Reset();
    cpu->a = a;
    cpu->x = x;
    cpu->y = y;
    cpu->s = s;
    mem->Write(0x8181, value);
    mem->Write(0x8000, opcode);
    mem->Write(0x8001, 0x80);
    mem->Write(0x8002, 0x81);
    cpu->x = 0x01;
    if (flags!=nullptr) cpu->p = *flags;
    cpu->Cycle();
    EXPECT_EQ(cpu->pc, 0x8003);
    EXPECT_FALSE(cpu->error);
}

void EXPECT_AbsoluteX(std::shared_ptr<IMemory> mem, uint8_t value){
    EXPECT_EQ(mem->Read(0x8181), value);
}

void ExecuteAbsoluteY(ICpu *cpu, std::shared_ptr<IMemory> mem, uint8_t opcode, uint8_t value, CpuFlags *flags, uint8_t a=0, uint8_t x=0, uint8_t y=0, uint8_t s=0xFD) {
    cpu->Reset();
    cpu->a = a;
    cpu->x = x;
    cpu->y = y;
    cpu->s = s;
    mem->Write(0x8181, value);
    mem->Write(0x8000, opcode);
    mem->Write(0x8001, 0x80);
    mem->Write(0x8002, 0x81);
    cpu->y = 0x01;
    if (flags!=nullptr) cpu->p = *flags;
    cpu->Cycle();
    EXPECT_EQ(cpu->pc, 0x8003);
    EXPECT_FALSE(cpu->error);
}

void EXPECT_AbsoluteY(std::shared_ptr<IMemory> mem, uint8_t value){
    EXPECT_EQ(mem->Read(0x8181), value);
}

void ExecuteIndirectX(ICpu *cpu, std::shared_ptr<IMemory> mem, uint8_t opcode, uint8_t value, CpuFlags *flags, uint8_t a=0, uint8_t x=0, uint8_t y=0, uint8_t s=0xFD) {
    cpu->Reset();
    cpu->a = a;
    cpu->x = x;
    cpu->y = y;
    cpu->s = s;
    mem->Write(0x001F, 0x80);
    mem->Write(0x0020, 0x81);
    mem->Write(0x8000, opcode);
    mem->Write(0x8001, 0x1E);
    mem->Write(0x8180, value);
    cpu->x = 0x01;
    if (flags!=nullptr) cpu->p = *flags;
    cpu->Cycle();
    EXPECT_EQ(cpu->pc, 0x8002);
    EXPECT_FALSE(cpu->error);
}

void EXPECT_IndirectX(std::shared_ptr<IMemory> mem, uint8_t value){
    EXPECT_EQ(mem->Read(0x8180), value);
}

void ExecuteIndirectY(ICpu *cpu, std::shared_ptr<IMemory> mem, uint8_t opcode, uint8_t value, CpuFlags *flags, uint8_t a=0, uint8_t x=0, uint8_t y=0, uint8_t s=0xFD) {
    cpu->Reset();
    cpu->a = a;
    cpu->x = x;
    cpu->y = y;
    cpu->s = s;
    mem->Write(0x001F, 0x80);
    mem->Write(0x0020, 0x81);
    mem->Write(0x8000, opcode);
    mem->Write(0x8001, 0x1F);
    mem->Write(0x8181, value);
    cpu->y = 0x01;
    if (flags!=nullptr) cpu->p = *flags;
    cpu->Cycle();
    EXPECT_EQ(cpu->pc, 0x8002);
    EXPECT_FALSE(cpu->error);
}

void EXPECT_IndirectY(std::shared_ptr<IMemory> mem, uint8_t value){
    EXPECT_EQ(mem->Read(0x8181), value);
}

void ExecuteJmpAbsolute(ICpu *cpu, std::shared_ptr<IMemory> mem, uint8_t opcode, CpuFlags *flags, uint8_t a=0, uint8_t x=0, uint8_t y=0, uint8_t s=0xFD) {
    cpu->Reset();
    cpu->a = a;
    cpu->x = x;
    cpu->y = y;
    cpu->s = s;
    mem->Write(0x8000, opcode);
    mem->Write(0x8001, 0x81);
    mem->Write(0x8002, 0x81);
    if (flags!=nullptr) cpu->p = *flags;
    cpu->Cycle();
    EXPECT_EQ(cpu->pc, 0x8181);
    EXPECT_FALSE(cpu->error);
}

void ExecuteJmpIndirect(ICpu *cpu, std::shared_ptr<IMemory> mem, uint8_t opcode, CpuFlags *flags, uint8_t a=0, uint8_t x=0, uint8_t y=0, uint8_t s=0xFD) {
    cpu->Reset();
    cpu->a = a;
    cpu->x = x;
    cpu->y = y;
    cpu->s = s;
    mem->Write(0x001F, 0x81);
    mem->Write(0x0020, 0x81);
    mem->Write(0x8000, opcode);
    mem->Write(0x8001, 0x1F);
    mem->Write(0x8002, 0x20);
    if (flags!=nullptr) cpu->p = *flags;
    cpu->Cycle();
    EXPECT_EQ(cpu->pc, 0x8181);
    EXPECT_FALSE(cpu->error);
}

void EXPECT_nvidzc(ICpu *cpu, bool n = false, bool v = false, bool d = false, bool i = true, bool z = false, bool c = false) {
    EXPECT_EQ(cpu->p.v, v);
    EXPECT_EQ(cpu->p.c, c);
    EXPECT_EQ(cpu->p.n, n);
    EXPECT_EQ(cpu->p.z, z);
    EXPECT_EQ(cpu->p.d, d);
    EXPECT_EQ(cpu->p.i, i);
}

namespace {
    class CpuTest : public ::testing::Test {
    protected:
        virtual void SetUp() {
            mem = std::make_shared<MemoryStub>();
            cpu = new Cpu(mem);
        }

        std::shared_ptr<IMemory> mem;
        ICpu *cpu;
    };
}


#endif //LITTLEPNES_CPUTESTS_H
