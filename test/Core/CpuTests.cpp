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
    
}

TEST_F(CpuTest, LdxZeroPageYAddressing) {
    ExecuteZeroPageY(cpu, mem, 0xB6, 0xAB, nullptr, 0x00, 0x00, 0x01);
    EXPECT_EQ(cpu->x, 0xAB);
    EXPECT_nvidzc(cpu, true, false, false, true, false, false);
}
