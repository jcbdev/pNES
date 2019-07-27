//
// Created by jimbo on 23/12/18.
//

#ifndef PNES_PPUSTUB_H
#define PNES_PPUSTUB_H


#include "../../src/Core/Ppu.h"

class PpuStub : public IPpu {
public:
    explicit PpuStub(ISystem* system);

    void Reset();

    uint8_t Read(uint16_t addr);
    void Write(uint16_t addr, uint8_t data);
    uint8_t ReadRegister(uint16_t addr);
    void WriteRegister(uint16_t addr, uint8_t data);
    void WriteDMA(uint8_t value);
    void Snapshot();

    void Step();

    uint8_t PPUCTRL();
    uint8_t PPUMASK();
    uint8_t PPUSTATUS();
    uint8_t OAMADDR();
    uint16_t V();
    uint16_t X();
    uint16_t T();
    bool FrameToggle();
    uint8_t Buffer();
    uint16_t Dot();
    int16_t Scanline();

    uint32_t* ScreenBuffer();
    uint8_t* TestBuffer();
};


#endif //PNES_PPUSTUB_H
