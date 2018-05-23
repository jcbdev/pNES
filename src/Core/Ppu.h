//
// Created by James Barker on 08/01/2018.
//

#ifndef LITTLEPNES_PPU_H
#define LITTLEPNES_PPU_H

#include <cstdint>
#include "System.h"

struct Raster {
    uint16_t nametable;
    uint16_t attribute;
    uint16_t tileLo;
    uint16_t tileHi;

    uint8_t iterator;
    uint8_t counter;

    struct OAM {
        uint8_t id;
        uint8_t y;
        uint8_t tile;
        uint8_t attributes;
        uint8_t x;

        uint8_t tileLo;
        uint8_t tileHi;
    } oam[8], soam[8];
};

class IPpu {
public:
    explicit IPpu(ISystem* system);

    int32_t clocks;
    bool render;

    virtual void Reset() = 0;

    virtual uint8_t Read(uint16_t addr) = 0;
    virtual void Write(uint16_t addr, uint8_t data) = 0;
    virtual uint8_t ReadRegister(uint16_t addr) = 0;
    virtual void WriteRegister(uint16_t addr, uint8_t data) = 0;
    virtual void WriteDMA(uint8_t value) = 0;

    //virtual void RasterPixel(unsigned x) = 0;
    //virtual void RasterSprite() = 0;
    virtual void Step() = 0;

    virtual uint8_t PPUCTRL() = 0;
    virtual uint8_t PPUMASK() = 0;
    virtual uint8_t PPUSTATUS() = 0;
    virtual uint8_t OAMADDR() = 0;
    virtual uint16_t V() = 0;
    virtual uint16_t X() = 0;
    virtual uint16_t T() = 0;
    virtual bool FrameToggle() = 0;
    virtual uint8_t Buffer() = 0;
    virtual uint16_t Dot() = 0;
    virtual int16_t Scanline() = 0;

    virtual uint32_t* ScreenBuffer() = 0;

protected:
    ISystem* _system;
};

#endif //LITTLEPNES_PPU_H
