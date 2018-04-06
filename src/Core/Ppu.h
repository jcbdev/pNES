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
    uint16_t dot;
    int16_t scanline;

    uint16_t vaddr;
    uint16_t xaddr;
    uint16_t tileAddr;
    bool frameToggle;
    uint8_t buffer;

    virtual void Reset() = 0;
    virtual uint8_t CiramRead(uint16_t addr) = 0;
    virtual void CiramWrite(uint16_t addr, uint8_t data) = 0;
    virtual uint8_t CgramRead(uint16_t addr) = 0;
    virtual void CgramWrite(uint16_t addr, uint8_t data) = 0;
    virtual uint8_t ChrLoad(uint16_t addr) = 0;

    virtual uint8_t Read(uint16_t addr) = 0;
    virtual void Write(uint16_t addr, uint8_t data) = 0;

    virtual void RasterPixel(unsigned x) = 0;
    virtual void RasterSprite() = 0;
    virtual void Cycle() = 0;

    virtual uint8_t PPUCTRL() = 0;
    virtual uint8_t PPUMASK() = 0;
    virtual uint8_t PPUSTATUS() = 0;
    virtual uint8_t OAMADDR() = 0;

    virtual uint32_t* ScreenBuffer() = 0;
    virtual uint32_t* ChrData() = 0;
protected:
    ISystem* _system;
};

class Ppu : public IPpu {
public:
    explicit Ppu(ISystem *system);

    void Reset() override;
    uint8_t CiramRead(uint16_t addr) override;
    void CiramWrite(uint16_t addr, uint8_t data) override;
    uint8_t CgramRead(uint16_t addr) override;
    void CgramWrite(uint16_t addr, uint8_t data) override;
    uint8_t ChrLoad(uint16_t addr) override;

    uint8_t Read(uint16_t addr) override;
    void Write(uint16_t addr, uint8_t data) override;

    void RasterPixel(unsigned x) override;
    void RasterSprite() override;
    void Cycle() override;

    uint8_t PPUCTRL() override;
    uint8_t PPUMASK() override;
    uint8_t PPUSTATUS() override;
    uint8_t OAMADDR() override;

    uint32_t* ScreenBuffer() override;
    uint32_t* ChrData() override;
private:
    bool _rasterEnable();
    uint8_t _spriteHeight();
    uint8_t _scrollY();
    uint8_t _scrollX();
    void _scrollXIncrement();
    void _scrollYIncrement();
    void _scanlineIncrement();
    void _frameEdge();
    void _scanlineEdge();

    void _addClocks();
    void _preRenderScanline();
    void _visibleDot();
    void _fetchSpriteDataForNextScanline();
    void _fetchTileDataForNextScanline();
    void _fetchNameTable();
    void _visibleScanline();
    void _verticalBlankingLine();
    void _initPalette();
    uint8_t _clamp(unsigned x);

    uint32_t _screenbuffer[256 * 261];
    uint8_t _ciram[2048];
    uint8_t _cgram[32];
    uint8_t _oam[256];


    bool _latch;
    uint8_t _mdr;

    //$2000 - PPUCTRL
    bool _nmiEnable;
    bool _masterSlave;
    bool _spriteHeightMode;
    uint8_t _bgTileSelect;
    uint8_t _spriteTileSelect;
    uint8_t _incrementMode;
    uint8_t _nametableSelect;

    //$2001 - PPUMASK
    uint8_t _bgrEmphasis;
    bool _spriteEnable;
    bool _bgEnable;
    bool _spriteLColEnable;
    bool _bgLColEnable;
    bool _grayscale;

    //$2002 - PPUSTATUS
    bool _vblank;
    bool _spriteZeroHit;
    bool _spriteOverflow;

    //$2003 - OAMADDR
    uint8_t _oamAddr;

    Raster raster;
    unsigned _nametableLatch;
    unsigned _tileAddrLatch;
    unsigned _attributeLatch;
    unsigned _tileHiLatch;
    unsigned _tileLoLatch;

    uint8_t _palette[256][3];
    uint32_t _chrData[0x2000];
    uint32_t _totalCycles;

    const uint32_t paletteRGB[64] = {
            0x7c7c7c,
            0x0000fc,
            0x0000bc,
            0x4428bc,
            0x940084,
            0xa80020,
            0xa81000,
            0x881400,
            0x503000,
            0x007800,
            0x006800,
            0x005800,
            0x004058,
            0x000000,
            0x000000,
            0x000000,
            0xbcbcbc,
            0x0078f8,
            0x0058f8,
            0x6844fc,
            0xd800cc,
            0xe40058,
            0xf83800,
            0xe45c10,
            0xac7c00,
            0x00b800,
            0x00a800,
            0x00a844,
            0x008888,
            0x000000,
            0x000000,
            0x000000,
            0xf8f8f8,
            0x3cbcfc,
            0x6888fc,
            0x9878f8,
            0xf878f8,
            0xf85898,
            0xf87858,
            0xfca044,
            0xf8b800,
            0xb8f818,
            0x58d854,
            0x58f898,
            0x00e8d8,
            0x787878,
            0x000000,
            0x000000,
            0xfcfcfc,
            0xa4e4fc,
            0xb8b8f8,
            0xd8b8f8,
            0xf8b8f8,
            0xf8a4c0,
            0xf0d0b0,
            0xfce0a8,
            0xf8d878,
            0xd8f878,
            0xb8f8b8,
            0xb8f8d8,
            0x00fcfc,
            0xf8d8f8,
            0x000000,
            0x000000
    };
};


#endif //LITTLEPNES_PPU_H
