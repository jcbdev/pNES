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

    virtual void Reset() = 0;
    virtual void Tick() = 0;
    virtual uint8_t CiramRead(uint16_t addr) = 0;
    virtual void CiramWrite(uint16_t addr, uint8_t data) = 0;
    virtual uint8_t CgramRead(uint16_t addr) = 0;
    virtual void CgramWrite(uint16_t addr, uint8_t data) = 0;
    virtual uint8_t ChrLoad(uint16_t addr) = 0;

    virtual uint8_t Read(uint16_t addr) = 0;
    virtual void Write(uint16_t addr, uint8_t data) = 0;

    virtual void RasterPixel(unsigned x) = 0;
    virtual void RasterSprite() = 0;
    virtual void RasterScanline() = 0;

    virtual uint16_t* ScreenBuffer() = 0;
protected:
    ISystem* _system;
};

class Ppu : public IPpu {
public:
    explicit Ppu(ISystem *system);

    void Reset() override;
    void Tick() override;
    uint8_t CiramRead(uint16_t addr) override;
    void CiramWrite(uint16_t addr, uint8_t data) override;
    uint8_t CgramRead(uint16_t addr) override;
    void CgramWrite(uint16_t addr, uint8_t data) override;
    uint8_t ChrLoad(uint16_t addr) override;

    uint8_t Read(uint16_t addr) override;
    void Write(uint16_t addr, uint8_t data) override;

    void RasterPixel(unsigned x) override;
    void RasterSprite() override;
    void RasterScanline() override;

    uint16_t* ScreenBuffer() override;
private:
    bool _rasterEnable();
    uint8_t _spriteHeight();
    uint8_t _scrollY();
    uint8_t _scrollX();
    void _scrollXIncrement();
    void _scrollYIncrement();
    void _yIncrement();
    void _frameEdge();
    void _scanlineEdge();

    uint16_t _screenbuffer[256 * 262];
    uint8_t _ciram[2048];
    uint8_t _cgram[32];
    uint8_t _oam[256];

    bool _field;
    uint8_t _x;
    uint8_t _y;

    uint8_t _mbr;
    uint8_t _data;
    bool _latch;

    uint16_t _vaddr;
    uint16_t _xaddr;
    uint16_t _tileAddr;

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
};


#endif //LITTLEPNES_PPU_H
