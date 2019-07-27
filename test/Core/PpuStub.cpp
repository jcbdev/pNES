//
// Created by jimbo on 23/12/18.
//

#include "PpuStub.h"

PpuStub::PpuStub(ISystem *system) : IPpu(system) {

}

void PpuStub::Reset() {

}

uint8_t PpuStub::Read(uint16_t addr) {
    return 0;
}

void PpuStub::Write(uint16_t addr, uint8_t data) {

}

uint8_t PpuStub::ReadRegister(uint16_t addr) {
    return 0;
}

void PpuStub::WriteRegister(uint16_t addr, uint8_t data) {

}

void PpuStub::WriteDMA(uint8_t value) {

}

void PpuStub::Snapshot() {

}

void PpuStub::Step() {

}

uint8_t PpuStub::PPUCTRL() {
    return 0;
}

uint8_t PpuStub::PPUMASK() {
    return 0;
}

uint8_t PpuStub::PPUSTATUS() {
    return 0;
}

uint8_t PpuStub::OAMADDR() {
    return 0;
}

uint16_t PpuStub::V() {
    return 0;
}

uint16_t PpuStub::X() {
    return 0;
}

uint16_t PpuStub::T() {
    return 0;
}

bool PpuStub::FrameToggle() {
    return false;
}

uint8_t PpuStub::Buffer() {
    return 0;
}

uint16_t PpuStub::Dot() {
    return 0;
}

int16_t PpuStub::Scanline() {
    return 0;
}

uint32_t *PpuStub::ScreenBuffer() {
    return nullptr;
}

uint8_t *PpuStub::TestBuffer() {
    return nullptr;
}
