//
// Created by James Barker on 20/11/2016.
//

#include "CpuTests.h"

TEST_F(CpuTest, AdcSimple) {
    ExecuteImmediate(cpu, mem, 0x69, 0x1F, nullptr);
    EXPECT_EQ(cpu->a, 0x1F);
    EXPECT_nvdizc(cpu, false, false, false, true, false, false);
}

TEST_F(CpuTest, AdcNegative) {
    ExecuteImmediate(cpu, mem, 0x69, 0xAB, nullptr);
    EXPECT_EQ(cpu->a, 0xAB);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
}

TEST_F(CpuTest, AdcCarryOver) {
    CpuFlags p = nvdizc(false, false, false, true, false, true);
    ExecuteImmediate(cpu, mem, 0x69, 0x1F, &p);
    EXPECT_EQ(cpu->a, 0x20);
    EXPECT_nvdizc(cpu, false, false, false, true, false, false);
}

TEST_F(CpuTest, AdcOverflowAndOtherRegs) {
    CpuFlags p = nvdizc(false, false, true, false, false, true);
    ExecuteImmediate(cpu, mem, 0x69, 0x1E, &p, 0xE1);
    EXPECT_EQ(cpu->a, 0x00);
    EXPECT_nvdizc(cpu, false, false, true, false, true, true);
}

TEST_F(CpuTest, AdcOverflow) {
    ExecuteImmediate(cpu, mem, 0x69, 0x1F, nullptr, 0xFF);
    EXPECT_EQ(cpu->a, 0x1E);
    EXPECT_nvdizc(cpu, false, false, false, true, false, true);
}

TEST_F(CpuTest, AdcAllAddressingModes) {
    ExecuteImmediate(cpu, mem, 0x69, 0x1F, nullptr);
    EXPECT_EQ(cpu->a, 0x1F);
    ExecuteZeroPage(cpu, mem, 0x65, 0xAB, nullptr);
    EXPECT_EQ(cpu->a, 0xAB);
    ExecuteZeroPageX(cpu, mem, 0x75, 0xAB, nullptr, 0, 0x01);
    EXPECT_EQ(cpu->a, 0xAB);
    ExecuteAbsolute(cpu, mem, 0x6D, 0xAB, nullptr);
    EXPECT_EQ(cpu->a, 0xAB);
    ExecuteAbsoluteX(cpu, mem, 0x7D, 0xAB, nullptr, 0x00, 0x01);
    EXPECT_EQ(cpu->a, 0xAB);
    ExecuteAbsoluteY(cpu, mem, 0x79, 0xAB, nullptr, 0x00, 0x00, 0x01);
    EXPECT_EQ(cpu->a, 0xAB);
    ExecuteIndirectX(cpu, mem, 0x61, 0xAB, nullptr, 0x00, 0x01);
    EXPECT_EQ(cpu->a, 0xAB);
    ExecuteIndirectY(cpu, mem, 0x71, 0xAB, nullptr, 0x00, 0x00, 0x01);
    EXPECT_EQ(cpu->a, 0xAB);
}

TEST_F(CpuTest, AndSimple) {
    ExecuteImmediate(cpu, mem, 0x29, 0x01, nullptr, 0x11);
    EXPECT_EQ(cpu->a, 0x01);
    EXPECT_nvdizc(cpu, false, false, false, true, false, false);
}

TEST_F(CpuTest, AndZero) {
    ExecuteImmediate(cpu, mem, 0x29, 0x01, nullptr, 0x10);
    EXPECT_EQ(cpu->a, 0x00);
    EXPECT_nvdizc(cpu, false, false, false, true, true, false);
}

TEST_F(CpuTest, AndNegative) {
    ExecuteImmediate(cpu, mem, 0x29, 0x91, nullptr, 0x90);
    EXPECT_EQ(cpu->a, 0x90);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
}

TEST_F(CpuTest, AndAllAddressingModes) {
    ExecuteImmediate(cpu, mem, 0x29, 0x01, nullptr, 0x11);
    EXPECT_EQ(cpu->a, 0x01);
    ExecuteZeroPage(cpu, mem, 0x25, 0x01, nullptr, 0x11);
    EXPECT_EQ(cpu->a, 0x01);
    ExecuteZeroPageX(cpu, mem, 0x35, 0x01, nullptr, 0x11);
    EXPECT_EQ(cpu->a, 0x01);
    ExecuteAbsolute(cpu, mem, 0x2D, 0x01, nullptr, 0x11);
    EXPECT_EQ(cpu->a, 0x01);
    ExecuteAbsoluteX(cpu, mem, 0x3D, 0x01, nullptr, 0x11);
    EXPECT_EQ(cpu->a, 0x01);
    ExecuteAbsoluteY(cpu, mem, 0x39, 0x01, nullptr, 0x11);
    EXPECT_EQ(cpu->a, 0x01);
    ExecuteIndirectX(cpu, mem, 0x21, 0x01, nullptr, 0x11);
    EXPECT_EQ(cpu->a, 0x01);
    ExecuteIndirectY(cpu, mem, 0x31, 0x01, nullptr, 0x11);
    EXPECT_EQ(cpu->a, 0x01);
}

TEST_F(CpuTest, AslAccumlator) {
    ExecuteAccumulator(cpu, mem, 0x0A, nullptr, 0x01);
    EXPECT_EQ(cpu->a, 0x02);
    EXPECT_nvdizc(cpu, false, false, false, true, false, false);
}

TEST_F(CpuTest, AslNegative) {
    ExecuteAccumulator(cpu, mem, 0x0A, nullptr, 0x41);
    EXPECT_EQ(cpu->a, 0x82);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
}

TEST_F(CpuTest, AslOverflow) {
    ExecuteAccumulator(cpu, mem, 0x0A, nullptr, 0x81);
    EXPECT_EQ(cpu->a, 0x02);
    EXPECT_nvdizc(cpu, false, false, false, true, false, true);
}

TEST_F(CpuTest, AslZero) {
    ExecuteAccumulator(cpu, mem, 0x0A, nullptr, 0x80);
    EXPECT_EQ(cpu->a, 0x0);
    EXPECT_nvdizc(cpu, false, false, false, true, true, true);
}

TEST_F(CpuTest, AslAllAddressingModes) {
    ExecuteAccumulator(cpu, mem, 0x0A, nullptr, 0x01);
    EXPECT_EQ(cpu->a, 0x02);
    ExecuteZeroPage(cpu, mem, 0x06, 0x01, nullptr);
    EXPECT_ZeroPage(mem, 0x02);
    ExecuteZeroPageX(cpu, mem, 0x16, 0x01, nullptr);
    EXPECT_ZeroPageX(mem, 0x02);
    ExecuteAbsolute(cpu, mem, 0x0E, 0x01, nullptr);
    EXPECT_Absolute(mem, 0x02);
    ExecuteAbsoluteX(cpu, mem, 0x1E, 0x01, nullptr);
    EXPECT_AbsoluteX(mem, 0x02);
}

TEST_F(CpuTest, BitAll) {
    ExecuteZeroPage(cpu, mem, 0x24, 0xFF, nullptr, 0x1F);
    EXPECT_nvdizc(cpu, true, true, false, true, false, false);
    ExecuteZeroPage(cpu, mem, 0x24, 0xFF, nullptr, 0x00);
    EXPECT_nvdizc(cpu, true, true, false, true, true, false);
    ExecuteZeroPage(cpu, mem, 0x24, 0x00, nullptr, 0x00);
    EXPECT_nvdizc(cpu, false, false, false, true, true, false);
    ExecuteAbsolute(cpu, mem, 0x2C, 0xFF, nullptr, 0x1F);
    EXPECT_nvdizc(cpu, true, true, false, true, false, false);
}

TEST_F(CpuTest, Bpl) {
    CpuFlags p = nvdizc(false, false, false, true, false, false);
    ExecuteBranch(cpu, mem, 0x10, 0x40, &p, true);
    p = nvdizc(true, false, false, true, false, false);
    ExecuteBranch(cpu, mem, 0x10, 0x40, &p, false);
}

TEST_F(CpuTest, Bmi) {
    CpuFlags p = nvdizc(true, false, false, true, false, false);
    ExecuteBranch(cpu, mem, 0x30, 0x40, &p, true);
    p = nvdizc(false, false, false, true, false, false);
    ExecuteBranch(cpu, mem, 0x30, 0x40, &p, false);
}

TEST_F(CpuTest, Bvc) {
    CpuFlags p = nvdizc(false, false, false, true, false, false);
    ExecuteBranch(cpu, mem, 0x50, 0x40, &p, true);
    p = nvdizc(false, true, false, true, false, false);
    ExecuteBranch(cpu, mem, 0x50, 0x40, &p, false);
}

TEST_F(CpuTest, Bvs) {
    CpuFlags p = nvdizc(false, true, false, true, false, false);
    ExecuteBranch(cpu, mem, 0x70, 0x40, &p, true);
    p = nvdizc(false, false, false, true, false, false);
    ExecuteBranch(cpu, mem, 0x70, 0x40, &p, false);
}

TEST_F(CpuTest, Bcc) {
    CpuFlags p = nvdizc(false, false, false, true, false, false);
    ExecuteBranch(cpu, mem, 0x90, 0x40, &p, true);
    p = nvdizc(false, false, false, true, false, true);
    ExecuteBranch(cpu, mem, 0x90, 0x40, &p, false);
}

TEST_F(CpuTest, Bcs) {
    CpuFlags p = nvdizc(false, false, false, true, false, true);
    ExecuteBranch(cpu, mem, 0xB0, 0x40, &p, true);
    p = nvdizc(false, false, false, true, false, false);
    ExecuteBranch(cpu, mem, 0xB0, 0x40, &p, false);
}

TEST_F(CpuTest, Bne) {
    CpuFlags p = nvdizc(false, false, false, true, false, false);
    ExecuteBranch(cpu, mem, 0xD0, 0x40, &p, true);
    p = nvdizc(false, false, false, true, true, false);
    ExecuteBranch(cpu, mem, 0xD0, 0x40, &p, false);
}

TEST_F(CpuTest, Beq) {
    CpuFlags p = nvdizc(false, false, false, true, true, false);
    ExecuteBranch(cpu, mem, 0xF0, 0x40, &p, true);
    p = nvdizc(false, false, false, true, false, false);
    ExecuteBranch(cpu, mem, 0xF0, 0x40, &p, false);
}

TEST_F(CpuTest, Brk) {
    ExecuteBrk(cpu, mem, 0x00, 0x81, 0x82, nullptr);
}

TEST_F(CpuTest, CmpNegative){
    ExecuteImmediate(cpu, mem, 0xC9, 0x12, nullptr, 0x10);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
}

TEST_F(CpuTest, CmpZero){
    ExecuteImmediate(cpu, mem, 0xC9, 0x10, nullptr, 0x10);
    EXPECT_nvdizc(cpu, false, false, false, true, true, true);
}

TEST_F(CpuTest, CmpCarry){
    ExecuteImmediate(cpu, mem, 0xC9, 0x10, nullptr, 0x11);
    EXPECT_nvdizc(cpu, false, false, false, true, false, true);
}

TEST_F(CpuTest, CmpAllAddressingModes){
    ExecuteImmediate(cpu, mem, 0xC9, 0x12, nullptr, 0x10);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
    ExecuteZeroPage(cpu, mem, 0xC5, 0x12, nullptr, 0x10);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
    ExecuteZeroPageX(cpu, mem, 0xD5, 0x12, nullptr, 0x10);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
    ExecuteAbsolute(cpu, mem, 0xCD, 0x12, nullptr, 0x10);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
    ExecuteAbsoluteX(cpu, mem, 0xDD, 0x12, nullptr, 0x10);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
    ExecuteAbsoluteY(cpu, mem, 0xD9, 0x12, nullptr, 0x10);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
    ExecuteIndirectX(cpu, mem, 0xC1, 0x12, nullptr, 0x10);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
    ExecuteIndirectY(cpu, mem, 0xD1, 0x12, nullptr, 0x10);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
}

TEST_F(CpuTest, CpxNegative){
    ExecuteImmediate(cpu, mem, 0xE0, 0x12, nullptr, 0x00, 0x10);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
}

TEST_F(CpuTest, CpxZero){
    ExecuteImmediate(cpu, mem, 0xE0, 0x10, nullptr, 0x00, 0x10);
    EXPECT_nvdizc(cpu, false, false, false, true, true, true);
}

TEST_F(CpuTest, CpxCarry){
    ExecuteImmediate(cpu, mem, 0xE0, 0x10, nullptr, 0x00, 0x11);
    EXPECT_nvdizc(cpu, false, false, false, true, false, true);
}

TEST_F(CpuTest, CpxAllAddressingModes){
    ExecuteImmediate(cpu, mem, 0xE0, 0x12, nullptr, 0x10, 0x10);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
    ExecuteZeroPage(cpu, mem, 0xE4, 0x12, nullptr, 0x10, 0x10);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
    ExecuteAbsolute(cpu, mem, 0xEC, 0x12, nullptr, 0x10, 0x10);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
}

TEST_F(CpuTest, CpyNegative){
    ExecuteImmediate(cpu, mem, 0xC0, 0x12, nullptr, 0x00, 0x00, 0x10);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
}

TEST_F(CpuTest, CpyZero){
    ExecuteImmediate(cpu, mem, 0xC0, 0x10, nullptr, 0x00, 0x00, 0x10);
    EXPECT_nvdizc(cpu, false, false, false, true, true, true);
}

TEST_F(CpuTest, CpyCarry){
    ExecuteImmediate(cpu, mem, 0xC0, 0x10, nullptr, 0x00, 0x00, 0x11);
    EXPECT_nvdizc(cpu, false, false, false, true, false, true);
}

TEST_F(CpuTest, CpyAllAddressingModes){
    ExecuteImmediate(cpu, mem, 0xC0, 0x12, nullptr, 0x00, 0x00, 0x10);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
    ExecuteZeroPage(cpu, mem, 0xC4, 0x12, nullptr, 0x00, 0x00, 0x10);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
    ExecuteAbsolute(cpu, mem, 0xCC, 0x12, nullptr, 0x00, 0x00, 0x10);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
}

TEST_F(CpuTest, DecSimple){
    ExecuteZeroPage(cpu, mem, 0xC6, 0x11, nullptr);
    EXPECT_ZeroPage(mem, 0x10);
}

TEST_F(CpuTest, DecNegative){
    ExecuteZeroPage(cpu, mem, 0xC6, 0x00, nullptr);
    EXPECT_ZeroPage(mem, 0xFF);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
}

TEST_F(CpuTest, DecZero){
    ExecuteZeroPage(cpu, mem, 0xC6, 0x01, nullptr);
    EXPECT_ZeroPage(mem, 0x00);
    EXPECT_nvdizc(cpu, false, false, false, true, true, false);
}

TEST_F(CpuTest, DecAllAddressingModes){
    ExecuteZeroPage(cpu, mem, 0xC6, 0x11, nullptr);
    EXPECT_ZeroPage(mem, 0x10);
    ExecuteZeroPageX(cpu, mem, 0xD6, 0x11, nullptr);
    EXPECT_ZeroPageX(mem, 0x10);
    ExecuteAbsolute(cpu, mem, 0xCE, 0x11, nullptr);
    EXPECT_Absolute(mem, 0x10);
    ExecuteAbsoluteX(cpu, mem, 0xDE, 0x11, nullptr);
    EXPECT_AbsoluteX(mem, 0x10);
}

TEST_F(CpuTest, DexSimple) {
    ExecuteAccumulator(cpu, mem, 0xCA, nullptr, 0x00, 0x02);
    EXPECT_EQ(cpu->x, 0x01);
    EXPECT_nvdizc(cpu, false, false, false, true, false, false);
}

TEST_F(CpuTest, DexZero) {
    ExecuteAccumulator(cpu, mem, 0xCA, nullptr, 0x00, 0x01);
    EXPECT_EQ(cpu->x, 0x00);
    EXPECT_nvdizc(cpu, false, false, false, true, true, false);
}

TEST_F(CpuTest, DexNegative) {
    ExecuteAccumulator(cpu, mem, 0xCA, nullptr, 0x00, 0xFF);
    EXPECT_EQ(cpu->x, 0xFE);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
}

TEST_F(CpuTest, DeySimple) {
    ExecuteAccumulator(cpu, mem, 0x88, nullptr, 0x00, 0x00, 0x02);
    EXPECT_EQ(cpu->y, 0x01);
    EXPECT_nvdizc(cpu, false, false, false, true, false, false);
}

TEST_F(CpuTest, DeyZero) {
    ExecuteAccumulator(cpu, mem, 0x88, nullptr, 0x00, 0x00, 0x01);
    EXPECT_EQ(cpu->y, 0x00);
    EXPECT_nvdizc(cpu, false, false, false, true, true, false);
}

TEST_F(CpuTest, DeyNegative) {
    ExecuteAccumulator(cpu, mem, 0x88, nullptr, 0x00, 0x00, 0xFF);
    EXPECT_EQ(cpu->y, 0xFE);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
}

TEST_F(CpuTest, EorSimple) {
    ExecuteImmediate(cpu, mem, 0x49, 0xAA, nullptr, 0xAB);
    EXPECT_EQ(cpu->a, 0x01);
    EXPECT_nvdizc(cpu, false, false, false, true, false, false);
}

TEST_F(CpuTest, EorNegatve) {
    ExecuteImmediate(cpu, mem, 0x49, 0xFF, nullptr, 0x00);
    EXPECT_EQ(cpu->a, 0xFF);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
}

TEST_F(CpuTest, EorZero) {
    ExecuteImmediate(cpu, mem, 0x49, 0xFF, nullptr, 0xFF);
    EXPECT_EQ(cpu->a, 0x00);
    EXPECT_nvdizc(cpu, false, false, false, true, true, false);
}

TEST_F(CpuTest, EorAllAddressingModes) {
    ExecuteImmediate(cpu, mem, 0x49, 0xAA, nullptr, 0xAB);
    EXPECT_EQ(cpu->a, 0x01);
    ExecuteZeroPage(cpu, mem, 0x45, 0xAA, nullptr, 0xAB);
    EXPECT_EQ(cpu->a, 0x01);
    ExecuteZeroPageX(cpu, mem, 0x55, 0xAA, nullptr, 0xAB);
    EXPECT_EQ(cpu->a, 0x01);
    ExecuteAbsolute(cpu, mem, 0x4D, 0xAA, nullptr, 0xAB);
    EXPECT_EQ(cpu->a, 0x01);
    ExecuteAbsoluteX(cpu, mem, 0x5D, 0xAA, nullptr, 0xAB);
    EXPECT_EQ(cpu->a, 0x01);
    ExecuteAbsoluteY(cpu, mem, 0x59, 0xAA, nullptr, 0xAB);
    EXPECT_EQ(cpu->a, 0x01);
    ExecuteIndirectX(cpu, mem, 0x41, 0xAA, nullptr, 0xAB);
    EXPECT_EQ(cpu->a, 0x01);
    ExecuteIndirectY(cpu, mem, 0x51, 0xAA, nullptr, 0xAB);
    EXPECT_EQ(cpu->a, 0x01);
}

TEST_F(CpuTest, ClearFlags) {
    CpuFlags p = nvdizc(false, false, false, true, false, true);
    ExecuteFlagOperation(cpu, mem, 0x18, &p);
    EXPECT_nvdizc(cpu, false, false, false, true, false, false);
    p = nvdizc(false, false, false, true, false, false);
    ExecuteFlagOperation(cpu, mem, 0x58, &p);
    EXPECT_nvdizc(cpu, false, false, false, false, false, false);
    p = nvdizc(false, true, false, true, false, false);
    ExecuteFlagOperation(cpu, mem, 0xB8, &p);
    EXPECT_nvdizc(cpu, false, false, false, true, false, false);
    p = nvdizc(false, false, true, true, false, false);
    ExecuteFlagOperation(cpu, mem, 0xD8, &p);
    EXPECT_nvdizc(cpu, false, false, false, true, false, false);
}

TEST_F(CpuTest, SetFlags) {
    CpuFlags p = nvdizc(false, false, false, true, false, false);
    ExecuteFlagOperation(cpu, mem, 0x38, &p);
    EXPECT_nvdizc(cpu, false, false, false, true, false, true);
    p = nvdizc(false, false, false, false, false, false);
    ExecuteFlagOperation(cpu, mem, 0x78, &p);
    EXPECT_nvdizc(cpu, false, false, false, true, false, false);
    p = nvdizc(false, false, false, true, false, false);
    ExecuteFlagOperation(cpu, mem, 0xF8, &p);
    EXPECT_nvdizc(cpu, false, false, true, true, false, false);
}

TEST_F(CpuTest, IncSimple){
    ExecuteZeroPage(cpu, mem, 0xE6, 0x11, nullptr);
    EXPECT_ZeroPage(mem, 0x12);
}

TEST_F(CpuTest, IncNegative){
    ExecuteZeroPage(cpu, mem, 0xE6, 0xFE, nullptr);
    EXPECT_ZeroPage(mem, 0xFF);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
}

TEST_F(CpuTest, IncZero){
    ExecuteZeroPage(cpu, mem, 0xE6, 0xFF, nullptr);
    EXPECT_ZeroPage(mem, 0x00);
    EXPECT_nvdizc(cpu, false, false, false, true, true, false);
}

TEST_F(CpuTest, IncAllAddressingModes){
    ExecuteZeroPage(cpu, mem, 0xE6, 0x11, nullptr);
    EXPECT_ZeroPage(mem, 0x12);
    ExecuteZeroPageX(cpu, mem, 0xF6, 0x11, nullptr);
    EXPECT_ZeroPageX(mem, 0x12);
    ExecuteAbsolute(cpu, mem, 0xEE, 0x11, nullptr);
    EXPECT_Absolute(mem, 0x12);
    ExecuteAbsoluteX(cpu, mem, 0xFE, 0x11, nullptr);
    EXPECT_AbsoluteX(mem, 0x12);
}

TEST_F(CpuTest, InxSimple) {
    ExecuteAccumulator(cpu, mem, 0xE8, nullptr, 0x00, 0x01);
    EXPECT_EQ(cpu->x, 0x02);
    EXPECT_nvdizc(cpu, false, false, false, true, false, false);
}

TEST_F(CpuTest, InxZero) {
    ExecuteAccumulator(cpu, mem, 0xE8, nullptr, 0x00, 0xFF);
    EXPECT_EQ(cpu->x, 0x00);
    EXPECT_nvdizc(cpu, false, false, false, true, true, false);
}

TEST_F(CpuTest, InxNegative) {
    ExecuteAccumulator(cpu, mem, 0xE8, nullptr, 0x00, 0xFE);
    EXPECT_EQ(cpu->x, 0xFF);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
}

TEST_F(CpuTest, InySimple) {
    ExecuteAccumulator(cpu, mem, 0xC8, nullptr, 0x00, 0x00, 0x01);
    EXPECT_EQ(cpu->y, 0x02);
    EXPECT_nvdizc(cpu, false, false, false, true, false, false);
}

TEST_F(CpuTest, InyZero) {
    ExecuteAccumulator(cpu, mem, 0xC8, nullptr, 0x00, 0x00, 0xFF);
    EXPECT_EQ(cpu->y, 0x00);
    EXPECT_nvdizc(cpu, false, false, false, true, true, false);
}

TEST_F(CpuTest, InyNegative) {
    ExecuteAccumulator(cpu, mem, 0xC8, nullptr, 0x00, 0x00, 0xFE);
    EXPECT_EQ(cpu->y, 0xFF);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
}

TEST_F(CpuTest, JmpAbsolute){
    ExecuteJmpAbsolute(cpu, mem, 0x4c, nullptr);
}

TEST_F(CpuTest, JmpIndirect){
    ExecuteJmpIndirect(cpu, mem, 0x6c, nullptr);
}

TEST_F(CpuTest, Jsr){
    ExecuteJsr(cpu, mem, 0x20, nullptr);
}

TEST_F(CpuTest, LdaSimple) {
    ExecuteImmediate(cpu, mem, 0xA9, 0x01, nullptr);
    EXPECT_EQ(cpu->a, 0x01);
    EXPECT_nvdizc(cpu, false, false, false, true, false, false);
}

TEST_F(CpuTest, LdaNegative) {
    ExecuteImmediate(cpu, mem, 0xA9, 0xFF, nullptr);
    EXPECT_EQ(cpu->a, 0xFF);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
}

TEST_F(CpuTest, LdaZero) {
    ExecuteImmediate(cpu, mem, 0xA9, 0x00, nullptr);
    EXPECT_EQ(cpu->a, 0x00);
    EXPECT_nvdizc(cpu, false, false, false, true, true, false);
}

TEST_F(CpuTest, LdaAllAddressingModes) {
    ExecuteImmediate(cpu, mem, 0xA9, 0xAB, nullptr);
    EXPECT_EQ(cpu->a, 0xAB);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
    ExecuteZeroPage(cpu, mem, 0xA5, 0xAB, nullptr);
    EXPECT_EQ(cpu->a, 0xAB);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
    ExecuteZeroPageX(cpu, mem, 0xB5, 0xAB, nullptr);
    EXPECT_EQ(cpu->a, 0xAB);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
    ExecuteAbsolute(cpu, mem, 0xAD, 0xAB, nullptr);
    EXPECT_EQ(cpu->a, 0xAB);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
    ExecuteAbsoluteX(cpu, mem, 0xBD, 0xAB, nullptr);
    EXPECT_EQ(cpu->a, 0xAB);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
    ExecuteAbsoluteY(cpu, mem, 0xB9, 0xAB, nullptr);
    EXPECT_EQ(cpu->a, 0xAB);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
    ExecuteIndirectX(cpu, mem, 0xA1, 0xAB, nullptr);
    EXPECT_EQ(cpu->a, 0xAB);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
    ExecuteIndirectY(cpu, mem, 0xB1, 0xAB, nullptr);
    EXPECT_EQ(cpu->a, 0xAB);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
}

TEST_F(CpuTest, LdxSimple) {
    ExecuteImmediate(cpu, mem, 0xA2, 0x01, nullptr);
    EXPECT_EQ(cpu->x, 0x01);
    EXPECT_nvdizc(cpu, false, false, false, true, false, false);
}

TEST_F(CpuTest, LdxNegative) {
    ExecuteImmediate(cpu, mem, 0xA2, 0xFF, nullptr);
    EXPECT_EQ(cpu->x, 0xFF);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
}

TEST_F(CpuTest, LdxZero) {
    ExecuteImmediate(cpu, mem, 0xA2, 0x00, nullptr);
    EXPECT_EQ(cpu->x, 0x00);
    EXPECT_nvdizc(cpu, false, false, false, true, true, false);
}

TEST_F(CpuTest, LdxAllAddressingModes) {
    ExecuteImmediate(cpu, mem, 0xA2, 0xAB, nullptr);
    EXPECT_EQ(cpu->x, 0xAB);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
    ExecuteZeroPage(cpu, mem, 0xA6, 0xAB, nullptr);
    EXPECT_EQ(cpu->x, 0xAB);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
    ExecuteZeroPageY(cpu, mem, 0xB6, 0xAB, nullptr);
    EXPECT_EQ(cpu->x, 0xAB);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
    ExecuteAbsolute(cpu, mem, 0xAE, 0xAB, nullptr);
    EXPECT_EQ(cpu->x, 0xAB);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
    ExecuteAbsoluteY(cpu, mem, 0xBE, 0xAB, nullptr);
    EXPECT_EQ(cpu->x, 0xAB);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
}

TEST_F(CpuTest, LdySimple) {
    ExecuteImmediate(cpu, mem, 0xA0, 0x01, nullptr);
    EXPECT_EQ(cpu->y, 0x01);
    EXPECT_nvdizc(cpu, false, false, false, true, false, false);
}

TEST_F(CpuTest, LdyNegative) {
    ExecuteImmediate(cpu, mem, 0xA0, 0xFF, nullptr);
    EXPECT_EQ(cpu->y, 0xFF);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
}

TEST_F(CpuTest, LdyZero) {
    ExecuteImmediate(cpu, mem, 0xA0, 0x00, nullptr);
    EXPECT_EQ(cpu->y, 0x00);
    EXPECT_nvdizc(cpu, false, false, false, true, true, false);
}

TEST_F(CpuTest, LdyAllAddressingModes) {
    ExecuteImmediate(cpu, mem, 0xA0, 0xAB, nullptr);
    EXPECT_EQ(cpu->y, 0xAB);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
    ExecuteZeroPage(cpu, mem, 0xA4, 0xAB, nullptr);
    EXPECT_EQ(cpu->y, 0xAB);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
    ExecuteZeroPageY(cpu, mem, 0xB4, 0xAB, nullptr);
    EXPECT_EQ(cpu->y, 0xAB);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
    ExecuteAbsolute(cpu, mem, 0xAC, 0xAB, nullptr);
    EXPECT_EQ(cpu->y, 0xAB);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
    ExecuteAbsoluteX(cpu, mem, 0xBC, 0xAB, nullptr);
    EXPECT_EQ(cpu->y, 0xAB);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
}

TEST_F(CpuTest, LsrSimple) {
    ExecuteAccumulator(cpu, mem, 0x4A, nullptr, 0x04);
    EXPECT_EQ(cpu->a, 0x02);
    EXPECT_nvdizc(cpu, false, false, false, true, false, false);
}

TEST_F(CpuTest, LsrZeroCarryOver) {
    ExecuteAccumulator(cpu, mem, 0x4A, nullptr, 0x01);
    EXPECT_EQ(cpu->a, 0x00);
    EXPECT_nvdizc(cpu, false, false, false, true, true, true);
}

TEST_F(CpuTest, LsrNegative) {
    ExecuteAccumulator(cpu, mem, 0x4A, nullptr, 0xFF);
    EXPECT_EQ(cpu->a, 0x7F);
    EXPECT_nvdizc(cpu, false, false, false, true, false, true);
}

TEST_F(CpuTest, LsrAllAddressingModes) {
    ExecuteAccumulator(cpu, mem, 0x4A, nullptr, 0x04);
    EXPECT_EQ(cpu->a, 0x02);
    EXPECT_nvdizc(cpu, false, false, false, true, false, false);
    ExecuteZeroPage(cpu, mem, 0x46, 0x04, nullptr);
    EXPECT_nvdizc(cpu, false, false, false, true, false, false);
    ExecuteZeroPageX(cpu, mem, 0x56, 0x04, nullptr);
    EXPECT_nvdizc(cpu, false, false, false, true, false, false);
    ExecuteAbsolute(cpu, mem, 0x4E, 0x04, nullptr);
    EXPECT_nvdizc(cpu, false, false, false, true, false, false);
    ExecuteAbsoluteX(cpu, mem, 0x5E, 0x04, nullptr);
    EXPECT_nvdizc(cpu, false, false, false, true, false, false);
}

TEST_F(CpuTest, Nop) {
    ExecuteNop(cpu, mem, 0xEA, nullptr);
}

TEST_F(CpuTest, OraSimple) {
    ExecuteImmediate(cpu, mem, 0x09, 0x01, nullptr, 0x10);
    EXPECT_EQ(cpu->a, 0x11);
    EXPECT_nvdizc(cpu, false, false, false, true, false, false);
}

TEST_F(CpuTest, OraNegative) {
    ExecuteImmediate(cpu, mem, 0x09, 0xAB, nullptr, 0x04);
    EXPECT_EQ(cpu->a, 0xAF);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
}

TEST_F(CpuTest, OraZero) {
    ExecuteImmediate(cpu, mem, 0x09, 0x00, nullptr, 0x00);
    EXPECT_EQ(cpu->a, 0x00);
    EXPECT_nvdizc(cpu, false, false, false, true, true, false);
}

TEST_F(CpuTest, OraAllAddressingModes) {
    ExecuteImmediate(cpu, mem, 0x09, 0x01, nullptr, 0x10);
    EXPECT_EQ(cpu->a, 0x11);
    EXPECT_nvdizc(cpu, false, false, false, true, false, false);
    ExecuteZeroPage(cpu, mem, 0x05, 0x01, nullptr, 0x10);
    EXPECT_EQ(cpu->a, 0x11);
    EXPECT_nvdizc(cpu, false, false, false, true, false, false);
    ExecuteZeroPageX(cpu, mem, 0x15, 0x01, nullptr, 0x10);
    EXPECT_EQ(cpu->a, 0x11);
    EXPECT_nvdizc(cpu, false, false, false, true, false, false);
    ExecuteAbsolute(cpu, mem, 0x0D, 0x01, nullptr, 0x10);
    EXPECT_EQ(cpu->a, 0x11);
    EXPECT_nvdizc(cpu, false, false, false, true, false, false);
    ExecuteAbsoluteX(cpu, mem, 0x1D, 0x01, nullptr, 0x10);
    EXPECT_EQ(cpu->a, 0x11);
    EXPECT_nvdizc(cpu, false, false, false, true, false, false);
    ExecuteAbsoluteY(cpu, mem, 0x19, 0x01, nullptr, 0x10);
    EXPECT_EQ(cpu->a, 0x11);
    EXPECT_nvdizc(cpu, false, false, false, true, false, false);
    ExecuteIndirectX(cpu, mem, 0x01, 0x01, nullptr, 0x10);
    EXPECT_EQ(cpu->a, 0x11);
    EXPECT_nvdizc(cpu, false, false, false, true, false, false);
    ExecuteIndirectY(cpu, mem, 0x11, 0x01, nullptr, 0x10);
    EXPECT_EQ(cpu->a, 0x11);
    EXPECT_nvdizc(cpu, false, false, false, true, false, false);
}

TEST_F(CpuTest, RolSimple) {
    ExecuteAccumulator(cpu, mem, 0x2A, nullptr, 0x01);
    EXPECT_EQ(cpu->a, 0x02);
    EXPECT_nvdizc(cpu, false, false, false, true, false, false);
}

TEST_F(CpuTest, RolZero) {
    ExecuteAccumulator(cpu, mem, 0x2A, nullptr, 0x00);
    EXPECT_EQ(cpu->a, 0x00);
    EXPECT_nvdizc(cpu, false, false, false, true, true, false);
}

TEST_F(CpuTest, RolNegative) {
    ExecuteAccumulator(cpu, mem, 0x2A, nullptr, 0x7F);
    EXPECT_EQ(cpu->a, 0xFE);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
}

TEST_F(CpuTest, RolCarry) {
    ExecuteAccumulator(cpu, mem, 0x2A, nullptr, 0xFE);
    EXPECT_EQ(cpu->a, 0xFC);
    EXPECT_nvdizc(cpu, true, false, false, true, false, true);
}

TEST_F(CpuTest, RolAllAddressingModes){
    ExecuteAccumulator(cpu, mem, 0x2A, nullptr, 0x01);
    EXPECT_EQ(cpu->a, 0x02);
    EXPECT_nvdizc(cpu, false, false, false, true, false, false);
    ExecuteZeroPage(cpu, mem, 0x26, 0x01, nullptr);
    EXPECT_ZeroPage(mem, 0x02);
    EXPECT_nvdizc(cpu, false, false, false, true, false, false);
    ExecuteZeroPageX(cpu, mem, 0x36, 0x01, nullptr);
    EXPECT_ZeroPageX(mem, 0x02);
    EXPECT_nvdizc(cpu, false, false, false, true, false, false);
    ExecuteAbsolute(cpu, mem, 0x2E, 0x01, nullptr);
    EXPECT_Absolute(mem, 0x02);
    EXPECT_nvdizc(cpu, false, false, false, true, false, false);
    ExecuteAbsoluteX(cpu, mem, 0x3E, 0x01, nullptr);
    EXPECT_AbsoluteX(mem, 0x02);
    EXPECT_nvdizc(cpu, false, false, false, true, false, false);
}

TEST_F(CpuTest, RorSimple) {
    ExecuteAccumulator(cpu, mem, 0x6A, nullptr, 0x02);
    EXPECT_EQ(cpu->a, 0x01);
    EXPECT_nvdizc(cpu, false, false, false, true, false, false);
}

TEST_F(CpuTest, RorZero) {
    ExecuteAccumulator(cpu, mem, 0x6A, nullptr, 0x01);
    EXPECT_EQ(cpu->a, 0x00);
    EXPECT_nvdizc(cpu, false, false, false, true, true, true);
}

TEST_F(CpuTest, RorNegative) {
    CpuFlags p = nvdizc(false, false, false, true, false, true);
    ExecuteAccumulator(cpu, mem, 0x6A, &p, 0xFF);
    EXPECT_EQ(cpu->a, 0xFF);
    EXPECT_nvdizc(cpu, true, false, false, true, false, true);
}

TEST_F(CpuTest, RorAllAddressingModes){
    ExecuteAccumulator(cpu, mem, 0x6A, nullptr, 0x02);
    EXPECT_EQ(cpu->a, 0x01);
    EXPECT_nvdizc(cpu, false, false, false, true, false, false);
    ExecuteZeroPage(cpu, mem, 0x66, 0x02, nullptr);
    EXPECT_ZeroPage(mem, 0x01);
    EXPECT_nvdizc(cpu, false, false, false, true, false, false);
    ExecuteZeroPageX(cpu, mem, 0x76, 0x02, nullptr);
    EXPECT_ZeroPageX(mem, 0x01);
    EXPECT_nvdizc(cpu, false, false, false, true, false, false);
    ExecuteAbsolute(cpu, mem, 0x6E, 0x02, nullptr);
    EXPECT_Absolute(mem, 0x01);
    EXPECT_nvdizc(cpu, false, false, false, true, false, false);
    ExecuteAbsoluteX(cpu, mem, 0x7E, 0x02, nullptr);
    EXPECT_AbsoluteX(mem, 0x01);
    EXPECT_nvdizc(cpu, false, false, false, true, false, false);
}

TEST_F(CpuTest, Rti) {
    CpuFlags p = nvdizc(false, false, false, true, true, true);
    ExecuteRti(cpu, mem, 0x40, 0x8050, &p);
}

TEST_F(CpuTest, Rts) {
    CpuFlags p = nvdizc(false, false, false, true, true, true);
    ExecuteRts(cpu, mem, 0x60, 0x8050, &p);
}

TEST_F(CpuTest, SbcBorrow){
    ExecuteImmediate(cpu, mem, 0xE9, 0x01, nullptr, 0x1F);
    EXPECT_EQ(cpu->a, 0x1D);
    EXPECT_nvdizc(cpu, false, false, false, true, false, true);
}

TEST_F(CpuTest, SbcSubtract){
    CpuFlags p = nvdizc(false, false, false, true, false, true);
    ExecuteImmediate(cpu, mem, 0xE9, 0x01, &p, 0x1F);
    EXPECT_EQ(cpu->a, 0x1E);
    EXPECT_nvdizc(cpu, false, false, false, true, false, true);
}

TEST_F(CpuTest, SbcNegative){
    CpuFlags p = nvdizc(false, false, false, true, false, true);
    ExecuteImmediate(cpu, mem, 0xE9, 0x01, &p, 0x00);
    EXPECT_EQ(cpu->a, 0xFF);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
}

TEST_F(CpuTest, SbcOverflow){
    ExecuteImmediate(cpu, mem, 0xE9, 0x01, nullptr, 0xFF);
    EXPECT_EQ(cpu->a, 0xFD);
    EXPECT_nvdizc(cpu, true, false, false, true, false, true);
}

TEST_F(CpuTest, SbcAllAddressingModes) {
    ExecuteImmediate(cpu, mem, 0xE9, 0x01, nullptr, 0x1F);
    EXPECT_EQ(cpu->a, 0x1D);
    EXPECT_nvdizc(cpu, false, false, false, true, false, true);
    ExecuteZeroPage(cpu, mem, 0xE5, 0x01, nullptr, 0x1F);
    EXPECT_EQ(cpu->a, 0x1D);
    EXPECT_nvdizc(cpu, false, false, false, true, false, true);
    ExecuteZeroPageX(cpu, mem, 0xF5, 0x01, nullptr, 0x1F);
    EXPECT_EQ(cpu->a, 0x1D);
    EXPECT_nvdizc(cpu, false, false, false, true, false, true);
    ExecuteAbsolute(cpu, mem, 0xED, 0x01, nullptr, 0x1F);
    EXPECT_EQ(cpu->a, 0x1D);
    EXPECT_nvdizc(cpu, false, false, false, true, false, true);
    ExecuteAbsoluteX(cpu, mem, 0xFD, 0x01, nullptr, 0x1F);
    EXPECT_EQ(cpu->a, 0x1D);
    EXPECT_nvdizc(cpu, false, false, false, true, false, true);
    ExecuteAbsoluteY(cpu, mem, 0xF9, 0x01, nullptr, 0x1F);
    EXPECT_EQ(cpu->a, 0x1D);
    EXPECT_nvdizc(cpu, false, false, false, true, false, true);
    ExecuteIndirectX(cpu, mem, 0xE1, 0x01, nullptr, 0x1F);
    EXPECT_EQ(cpu->a, 0x1D);
    EXPECT_nvdizc(cpu, false, false, false, true, false, true);
    ExecuteIndirectY(cpu, mem, 0xF1, 0x01, nullptr, 0x1F);
    EXPECT_EQ(cpu->a, 0x1D);
    EXPECT_nvdizc(cpu, false, false, false, true, false, true);
}

TEST_F(CpuTest, StaAllAddressingModes){
    ExecuteZeroPage(cpu, mem, 0x85, 0x00, nullptr, 0x50);
    EXPECT_ZeroPage(mem, 0x50);
    ExecuteZeroPageX(cpu, mem, 0x95, 0x00, nullptr, 0x50);
    EXPECT_ZeroPageX(mem, 0x50);
    ExecuteAbsolute(cpu, mem, 0x8D, 0x00, nullptr, 0x50);
    EXPECT_Absolute(mem, 0x50);
    ExecuteAbsoluteX(cpu, mem, 0x9D, 0x00, nullptr, 0x50);
    EXPECT_AbsoluteX(mem, 0x50);
    ExecuteAbsoluteY(cpu, mem, 0x99, 0x00, nullptr, 0x50);
    EXPECT_AbsoluteY(mem, 0x50);
    ExecuteIndirectX(cpu, mem, 0x81, 0x00, nullptr, 0x50);
    EXPECT_IndirectX(mem, 0x50);
    ExecuteIndirectY(cpu, mem, 0x91, 0x00, nullptr, 0x50);
    EXPECT_IndirectY(mem, 0x50);
}

TEST_F(CpuTest, StxAllAddressingModes){
    ExecuteZeroPage(cpu, mem, 0x86, 0x00, nullptr, 0x00, 0x50);
    EXPECT_ZeroPage(mem, 0x50);
    ExecuteZeroPageY(cpu, mem, 0x96, 0x00, nullptr, 0x00, 0x50);
    EXPECT_ZeroPageY(mem, 0x50);
    ExecuteAbsolute(cpu, mem, 0x8E, 0x00, nullptr, 0x00, 0x50);
    EXPECT_Absolute(mem, 0x50);
}

TEST_F(CpuTest, StyAllAddressingModes){
    ExecuteZeroPage(cpu, mem, 0x84, 0x00, nullptr, 0x00, 0x00, 0x50);
    EXPECT_ZeroPage(mem, 0x50);
    ExecuteZeroPageX(cpu, mem, 0x94, 0x00, nullptr, 0x00, 0x00, 0x50);
    EXPECT_ZeroPageX(mem, 0x50);
    ExecuteAbsolute(cpu, mem, 0x8C, 0x00, nullptr, 0x00, 0x00, 0x50);
    EXPECT_Absolute(mem, 0x50);
}

TEST_F(CpuTest, TaxSimple) {
    ExecuteAccumulator(cpu, mem, 0xAA, nullptr, 0x01);
    EXPECT_EQ(cpu->x, 0x01);
    EXPECT_nvdizc(cpu, false, false, false, true, false, false);
}

TEST_F(CpuTest, TaxZero) {
    ExecuteAccumulator(cpu, mem, 0xAA, nullptr, 0x00, 0x01);
    EXPECT_EQ(cpu->x, 0x00);
    EXPECT_nvdizc(cpu, false, false, false, true, true, false);
}

TEST_F(CpuTest, TaxNegative) {
    ExecuteAccumulator(cpu, mem, 0xAA, nullptr, 0xFF);
    EXPECT_EQ(cpu->x, 0xFF);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
}

TEST_F(CpuTest, TxaSimple) {
    ExecuteAccumulator(cpu, mem, 0x8A, nullptr, 0x00, 0x01);
    EXPECT_EQ(cpu->a, 0x01);
    EXPECT_nvdizc(cpu, false, false, false, true, false, false);
}

TEST_F(CpuTest, TxaZero) {
    ExecuteAccumulator(cpu, mem, 0x8A, nullptr, 0x01, 0x00);
    EXPECT_EQ(cpu->a, 0x00);
    EXPECT_nvdizc(cpu, false, false, false, true, true, false);
}

TEST_F(CpuTest, TxaNegative) {
    ExecuteAccumulator(cpu, mem, 0x8A, nullptr, 0x00, 0xFF);
    EXPECT_EQ(cpu->a, 0xFF);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
}

TEST_F(CpuTest, TaySimple) {
    ExecuteAccumulator(cpu, mem, 0xA8, nullptr, 0x01);
    EXPECT_EQ(cpu->y, 0x01);
    EXPECT_nvdizc(cpu, false, false, false, true, false, false);
}

TEST_F(CpuTest, TayZero) {
    ExecuteAccumulator(cpu, mem, 0xA8, nullptr, 0x00);
    EXPECT_EQ(cpu->y, 0x00);
    EXPECT_nvdizc(cpu, false, false, false, true, true, false);
}

TEST_F(CpuTest, TayNegative) {
    ExecuteAccumulator(cpu, mem, 0xA8, nullptr, 0xFF);
    EXPECT_EQ(cpu->y, 0xFF);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
}

TEST_F(CpuTest, TyaSimple) {
    ExecuteAccumulator(cpu, mem, 0x98, nullptr, 0x00, 0x00, 0x01);
    EXPECT_EQ(cpu->a, 0x01);
    EXPECT_nvdizc(cpu, false, false, false, true, false, false);
}

TEST_F(CpuTest, TyaZero) {
    ExecuteAccumulator(cpu, mem, 0x98, nullptr, 0x01, 0x00, 0x00);
    EXPECT_EQ(cpu->a, 0x00);
    EXPECT_nvdizc(cpu, false, false, false, true, true, false);
}

TEST_F(CpuTest, TyaNegative) {
    ExecuteAccumulator(cpu, mem, 0x98, nullptr, 0x00, 0x00, 0xFF);
    EXPECT_EQ(cpu->a, 0xFF);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
}

TEST_F(CpuTest, TxsSimple) {
    ExecuteAccumulator(cpu, mem, 0x9A, nullptr, 0x00, 0x01);
    EXPECT_EQ(cpu->s, 0x01);
}

TEST_F(CpuTest, TsxSimple) {
    ExecuteAccumulator(cpu, mem, 0xBA, nullptr, 0x00, 0x00, 0x00, 0x01);
    EXPECT_EQ(cpu->x, 0x01);
    EXPECT_nvdizc(cpu, false, false, false, true, false, false);
}

TEST_F(CpuTest, TsxZero) {
    ExecuteAccumulator(cpu, mem, 0xBA, nullptr, 0x00, 0x01, 0x00, 0x00);
    EXPECT_EQ(cpu->x, 0x00);
    EXPECT_nvdizc(cpu, false, false, false, true, true, false);
}

TEST_F(CpuTest, TsxNegative) {
    ExecuteAccumulator(cpu, mem, 0xBA, nullptr, 0x00, 0x00, 0x00, 0xFF);
    EXPECT_EQ(cpu->x, 0xFF);
    EXPECT_nvdizc(cpu, true, false, false, true, false, false);
}