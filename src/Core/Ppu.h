//
// Created by James Barker on 08/01/2018.
//

#ifndef LITTLEPNES_PPU_H
#define LITTLEPNES_PPU_H

#include "Cpu.h"

class IPpu {
public:
    IPpu(ICpu* cpu);

    virtual uint8_t CiramRead(uint16_t addr) = 0;
    virtual void CiramWrite(uint16_t addr, uint8_t data) = 0;

    virtual uint8_t Read(uint16_t addr) = 0;
    virtual void Write(uint16_t addr, uint8_t data) = 0;

protected:
    ICpu* _cpu;
};

class Ppu : public IPpu {
public:
    Ppu(ICpu *cpu);
    uint8_t CiramRead(uint16_t addr);
    void CiramWrite(uint16_t addr, uint8_t data);

    uint8_t Read(uint16_t addr);
    void Write(uint16_t addr, uint8_t data);

private:
    uint16_t _screenbuffer[256 * 262];
    uint8_t _ciram[2048];
    uint8_t _cgram[32];
    uint8_t _oam[256];

    //$2000 - PPUCTRL
    bool _nmiEnable;
    bool _masterSlave;
    bool _spriteHeight;
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

    //$2004 - OAMDATA
    uint8_t _oamData;

    //$2005 - PPUSCROLL
    uint8_t _ppuScroll;

    //$2006 - PPUADDR
    uint8_t _ppuAddr;

    //$2007 - PPUDATA
    uint8_t _ppuData;

    //$4014 - OAMDMA
    uint8_t _oamDma;

};
#endif //LITTLEPNES_PPU_H
