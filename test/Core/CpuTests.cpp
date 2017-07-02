//
// Created by James Barker on 20/11/2016.
//

#include <gtest/gtest.h>
#include "../../src/Core/Memory.h"
#include "../../src/Core/Cpu.h"
#include "CpuTests.h"

TEST_F(CpuTest, AdcSimple) {
    ExecuteImmediate(cpu, mem, 0x69, 0x1F, nullptr);
    EXPECT_EQ(cpu->a, 0x1F);
    EXPECT_nvidzc(cpu, false, false, false, true, false, false);
}

TEST_F(CpuTest, AdcNegative) {
    ExecuteImmediate(cpu, mem, 0x69, 0xAB, nullptr);
    EXPECT_EQ(cpu->a, 0xAB);
    EXPECT_nvidzc(cpu, true, false, false, true, false, false);
}

TEST_F(CpuTest, AdcCarryOver) {
    CpuFlags p = nvdizc(false, false, false, true, false, true);
    ExecuteImmediate(cpu, mem, 0x69, 0x1F, &p);
    EXPECT_EQ(cpu->a, 0x20);
    EXPECT_nvidzc(cpu, false, false, false, true, false, false);
}

TEST_F(CpuTest, AdcOverflowAndOtherRegs) {
    CpuFlags p = nvdizc(false, false, true, false, false, true);
    ExecuteImmediate(cpu, mem, 0x69, 0x1E, &p, 0xE1);
    EXPECT_EQ(cpu->a, 0x00);
    EXPECT_nvidzc(cpu, false, false, true, false, true, true);
}

TEST_F(CpuTest, AdcOverflow) {
    ExecuteImmediate(cpu, mem, 0x69, 0x1F, nullptr, 0xFF);
    EXPECT_EQ(cpu->a, 0x1E);
    EXPECT_nvidzc(cpu, false, false, false, true, false, true);
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
    EXPECT_nvidzc(cpu, false, false, false, true, false, false);
}

TEST_F(CpuTest, AndZero) {
    ExecuteImmediate(cpu, mem, 0x29, 0x01, nullptr, 0x10);
    EXPECT_EQ(cpu->a, 0x00);
    EXPECT_nvidzc(cpu, false, false, false, true, true, false);
}

TEST_F(CpuTest, AndNegative) {
    ExecuteImmediate(cpu, mem, 0x29, 0x91, nullptr, 0x90);
    EXPECT_EQ(cpu->a, 0x90);
    EXPECT_nvidzc(cpu, true, false, false, true, false, false);
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
    EXPECT_nvidzc(cpu, false, false, false, true, false, false);
}

TEST_F(CpuTest, AslNegative) {
    ExecuteAccumulator(cpu, mem, 0x0A, nullptr, 0x41);
    EXPECT_EQ(cpu->a, 0x82);
    EXPECT_nvidzc(cpu, true, false, false, true, false, false);
}

TEST_F(CpuTest, AslOverflow) {
    ExecuteAccumulator(cpu, mem, 0x0A, nullptr, 0x81);
    EXPECT_EQ(cpu->a, 0x02);
    EXPECT_nvidzc(cpu, false, false, false, true, false, true);
}

TEST_F(CpuTest, AslZero) {
    ExecuteAccumulator(cpu, mem, 0x0A, nullptr, 0x80);
    EXPECT_EQ(cpu->a, 0x0);
    EXPECT_nvidzc(cpu, false, false, false, true, true, true);
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
    EXPECT_nvidzc(cpu, true, true, false, true, false, false);
    ExecuteZeroPage(cpu, mem, 0x24, 0xFF, nullptr, 0x00);
    EXPECT_nvidzc(cpu, true, true, false, true, true, false);
    ExecuteZeroPage(cpu, mem, 0x24, 0x00, nullptr, 0x00);
    EXPECT_nvidzc(cpu, false, false, false, true, true, false);
    ExecuteAbsolute(cpu, mem, 0x2C, 0xFF, nullptr, 0x1F);
    EXPECT_nvidzc(cpu, true, true, false, true, false, false);
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
    EXPECT_nvidzc(cpu, true, false, false, true, false, false);
}

TEST_F(CpuTest, CmpZero){
    ExecuteImmediate(cpu, mem, 0xC9, 0x10, nullptr, 0x10);
    EXPECT_nvidzc(cpu, false, false, false, true, true, true);
}

TEST_F(CpuTest, CmpCarry){
    ExecuteImmediate(cpu, mem, 0xC9, 0x10, nullptr, 0x11);
    EXPECT_nvidzc(cpu, false, false, false, true, false, true);
}

TEST_F(CpuTest, CmpAllAddressingModes){
    ExecuteImmediate(cpu, mem, 0xC9, 0x12, nullptr, 0x10);
    EXPECT_nvidzc(cpu, true, false, false, true, false, false);
    ExecuteZeroPage(cpu, mem, 0xC5, 0x12, nullptr, 0x10);
    EXPECT_nvidzc(cpu, true, false, false, true, false, false);
    ExecuteZeroPageX(cpu, mem, 0xD5, 0x12, nullptr, 0x10);
    EXPECT_nvidzc(cpu, true, false, false, true, false, false);
    ExecuteAbsolute(cpu, mem, 0xCD, 0x12, nullptr, 0x10);
    EXPECT_nvidzc(cpu, true, false, false, true, false, false);
    ExecuteAbsoluteX(cpu, mem, 0xDD, 0x12, nullptr, 0x10);
    EXPECT_nvidzc(cpu, true, false, false, true, false, false);
    ExecuteAbsoluteY(cpu, mem, 0xD9, 0x12, nullptr, 0x10);
    EXPECT_nvidzc(cpu, true, false, false, true, false, false);
    ExecuteIndirectX(cpu, mem, 0xC1, 0x12, nullptr, 0x10);
    EXPECT_nvidzc(cpu, true, false, false, true, false, false);
    ExecuteIndirectY(cpu, mem, 0xD1, 0x12, nullptr, 0x10);
    EXPECT_nvidzc(cpu, true, false, false, true, false, false);
}

TEST_F(CpuTest, CpxNegative){
    ExecuteImmediate(cpu, mem, 0xE0, 0x12, nullptr, 0x00, 0x10);
    EXPECT_nvidzc(cpu, true, false, false, true, false, false);
}

TEST_F(CpuTest, CpxZero){
    ExecuteImmediate(cpu, mem, 0xE0, 0x10, nullptr, 0x00, 0x10);
    EXPECT_nvidzc(cpu, false, false, false, true, true, true);
}

TEST_F(CpuTest, CpxCarry){
    ExecuteImmediate(cpu, mem, 0xE0, 0x10, nullptr, 0x00, 0x11);
    EXPECT_nvidzc(cpu, false, false, false, true, false, true);
}

TEST_F(CpuTest, CpxAllAddressingModes){
    ExecuteImmediate(cpu, mem, 0xE0, 0x12, nullptr, 0x10, 0x10);
    EXPECT_nvidzc(cpu, true, false, false, true, false, false);
    ExecuteZeroPage(cpu, mem, 0xE4, 0x12, nullptr, 0x10, 0x10);
    EXPECT_nvidzc(cpu, true, false, false, true, false, false);
    ExecuteAbsolute(cpu, mem, 0xEC, 0x12, nullptr, 0x10, 0x10);
    EXPECT_nvidzc(cpu, true, false, false, true, false, false);
}

TEST_F(CpuTest, CpyNegative){
    ExecuteImmediate(cpu, mem, 0xC0, 0x12, nullptr, 0x00, 0x00, 0x10);
    EXPECT_nvidzc(cpu, true, false, false, true, false, false);
}

TEST_F(CpuTest, CpyZero){
    ExecuteImmediate(cpu, mem, 0xC0, 0x10, nullptr, 0x00, 0x00, 0x10);
    EXPECT_nvidzc(cpu, false, false, false, true, true, true);
}

TEST_F(CpuTest, CpyCarry){
    ExecuteImmediate(cpu, mem, 0xC0, 0x10, nullptr, 0x00, 0x00, 0x11);
    EXPECT_nvidzc(cpu, false, false, false, true, false, true);
}

TEST_F(CpuTest, CpyAllAddressingModes){
    ExecuteImmediate(cpu, mem, 0xC0, 0x12, nullptr, 0x00, 0x00, 0x10);
    EXPECT_nvidzc(cpu, true, false, false, true, false, false);
    ExecuteZeroPage(cpu, mem, 0xC4, 0x12, nullptr, 0x00, 0x00, 0x10);
    EXPECT_nvidzc(cpu, true, false, false, true, false, false);
    ExecuteAbsolute(cpu, mem, 0xCC, 0x12, nullptr, 0x00, 0x00, 0x10);
    EXPECT_nvidzc(cpu, true, false, false, true, false, false);
}

TEST_F(CpuTest, DecSimple){
    ExecuteZeroPage(cpu, mem, 0xC6, 0x11, nullptr);
    EXPECT_ZeroPage(mem, 0x10);
}

TEST_F(CpuTest, DecNegative){
    ExecuteZeroPage(cpu, mem, 0xC6, 0x00, nullptr);
    EXPECT_ZeroPage(mem, 0xFF);
    EXPECT_nvidzc(cpu, true, false, false, true, false, false);
}

TEST_F(CpuTest, DecZero){
    ExecuteZeroPage(cpu, mem, 0xC6, 0x01, nullptr);
    EXPECT_ZeroPage(mem, 0x00);
    EXPECT_nvidzc(cpu, false, false, false, true, true, false);
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

TEST_F(CpuTest, EorSimple) {
    ExecuteImmediate(cpu, mem, 0x49, 0xAA, nullptr, 0xAB);
    EXPECT_EQ(cpu->a, 0x01);
    EXPECT_nvidzc(cpu, false, false, false, true, false, false);
}

TEST_F(CpuTest, EorNegatve) {
    ExecuteImmediate(cpu, mem, 0x49, 0xFF, nullptr, 0x00);
    EXPECT_EQ(cpu->a, 0xFF);
    EXPECT_nvidzc(cpu, true, false, false, true, false, false);
}

TEST_F(CpuTest, EorZero) {
    ExecuteImmediate(cpu, mem, 0x49, 0xFF, nullptr, 0xFF);
    EXPECT_EQ(cpu->a, 0x00);
    EXPECT_nvidzc(cpu, false, false, false, true, true, false);
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
    EXPECT_nvidzc(cpu, false, false, false, true, false, false);
    p = nvdizc(false, false, false, true, false, false);
    ExecuteFlagOperation(cpu, mem, 0x58, &p);
    EXPECT_nvidzc(cpu, false, false, false, false, false, false);
    p = nvdizc(false, true, false, true, false, false);
    ExecuteFlagOperation(cpu, mem, 0xB8, &p);
    EXPECT_nvidzc(cpu, false, false, false, true, false, false);
    p = nvdizc(false, false, true, true, false, false);
    ExecuteFlagOperation(cpu, mem, 0xD8, &p);
    EXPECT_nvidzc(cpu, false, false, false, true, false, false);
}

TEST_F(CpuTest, SetFlags) {
    CpuFlags p = nvdizc(false, false, false, true, false, false);
    ExecuteFlagOperation(cpu, mem, 0x38, &p);
    EXPECT_nvidzc(cpu, false, false, false, true, false, true);
    p = nvdizc(false, false, false, false, false, false);
    ExecuteFlagOperation(cpu, mem, 0x78, &p);
    EXPECT_nvidzc(cpu, false, false, false, true, false, false);
    p = nvdizc(false, false, false, true, false, false);
    ExecuteFlagOperation(cpu, mem, 0xF8, &p);
    EXPECT_nvidzc(cpu, false, false, true, true, false, false);
}

TEST_F(CpuTest, IncSimple){
    ExecuteZeroPage(cpu, mem, 0xE6, 0x11, nullptr);
    EXPECT_ZeroPage(mem, 0x12);
}

TEST_F(CpuTest, IncNegative){
    ExecuteZeroPage(cpu, mem, 0xE6, 0xFE, nullptr);
    EXPECT_ZeroPage(mem, 0xFF);
    EXPECT_nvidzc(cpu, true, false, false, true, false, false);
}

TEST_F(CpuTest, IncZero){
    ExecuteZeroPage(cpu, mem, 0xE6, 0xFF, nullptr);
    EXPECT_ZeroPage(mem, 0x00);
    EXPECT_nvidzc(cpu, false, false, false, true, true, false);
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



TEST_F(CpuTest, LdxZeroPageYAddressing) {
    ExecuteZeroPageY(cpu, mem, 0xB6, 0xAB, nullptr, 0x00, 0x00, 0x01);
    EXPECT_EQ(cpu->x, 0xAB);
    EXPECT_nvidzc(cpu, true, false, false, true, false, false);
}
