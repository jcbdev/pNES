//
// Created by James Barker on 06/04/2018.
//

#ifndef PNES_PPUNEW_H
#define PNES_PPUNEW_H

#include "Ppu.h"

struct spr {
    uint8_t a;
    uint8_t b;
};

class PpuNew : public IPpu {
public:
    explicit PpuNew(ISystem *system);

    void Reset() override;
    uint8_t CiramRead(uint16_t addr) override;
    void CiramWrite(uint16_t addr, uint8_t data) override;
    uint8_t CgramRead(uint16_t addr) override;
    void CgramWrite(uint16_t addr, uint8_t data) override;
    uint8_t ChrLoad(uint16_t addr) override;

    uint8_t Read(uint16_t addr) override;
    void Write(uint16_t addr, uint8_t data) override;

    void Step() override;

    uint8_t PPUCTRL() override;
    uint8_t PPUMASK() override;
    uint8_t PPUSTATUS() override;
    uint8_t OAMADDR() override;

    uint32_t* ScreenBuffer() override;

private:
    uint8_t _readPalette(uint16_t address);
    void _writePalette(uint16_t address, uint8_t value);
    void _writeControl(uint8_t value);
    void _writeMask(uint8_t value);
    void _writeOAMAddress(uint8_t value);
    uint8_t _readOAMData();
    uint8_t _readStatus();
    void _writeOAMData(uint8_t value);
    void _writeScroll(uint8_t value);
    void _writeAddress(uint8_t value);
    uint8_t _readData();
    void _writeData(uint8_t value);
    void _writeDMA(uint8_t value);
    void _incrementX();
    void _incrementY();
    void _copyX();
    void _copyY();
    void _nmiChange();
    void _setVerticalBlank();
    void _clearVerticalBlank();
    void _fetchNameTableByte();
    void _fetchAttributeTableByte();
    void _fetchLowTileByte();
    void _fetchHighTileByte();
    void _storeTileData();
    uint32_t _fetchTileData();
    uint8_t _backgroundPixel();
    spr _spritePixel();
    void _renderPixel();
    uint32_t _fetchSpritePattern(int i, int row);
    void _evaluateSprites();

    void tick();


    int Cycle;    // 0-340
    int ScanLine;    // 0-261, 0-239=visible, 240=post, 241-260=vblank, 261=pre
    uint64_t Frame; // frame counter

    // storage variables
    uint8_t paletteData[32];
    uint8_t nameTableData[2048];
    uint8_t oamData[256];

    // PPU registers
    uint16_t v; // current vram address (15 bit)
    uint16_t t; // temporary vram address (15 bit)
    uint8_t x;   // fine x scroll (3 bit)
    uint8_t w;   // write toggle (1 bit)
    uint8_t f;   // even/odd frame flag (1 bit)

    uint8_t reg;

    // NMI flags
    bool nmiOccurred;
    bool nmiOutput;
    bool nmiPrevious;
    uint8_t nmiDelay;

    // background temporary variables
    uint8_t nameTableByte;
    uint8_t attributeTableByte;
    uint8_t lowTileByte;
    uint8_t highTileByte;
    uint64_t tileData;

    // sprite temporary variables
    int spriteCount;
    uint32_t spritePatterns[8];
    uint8_t spritePositions[8];
    uint8_t spritePriorities[8];
    uint8_t spriteIndexes[8];

    // $2000 PPUCTRL
    uint8_t flagNameTable; // 0: $2000; 1: $2400; 2: $2800; 3: $2C00
    uint8_t flagIncrement; // 0: add 1; 1: add 32
    uint8_t flagSpriteTable; // 0: $0000; 1: $1000; ignored in 8x16 mode
    uint8_t flagBackgroundTable; // 0: $0000; 1: $1000
    uint8_t flagSpriteSize; // 0: 8x8; 1: 8x16
    uint8_t flagMasterSlave; // 0: read EXT; 1: write EXT

    // $2001 PPUMASK
    uint8_t flagGrayscale; // 0: color; 1: grayscale
    uint8_t flagShowLeftBackground; // 0: hide; 1: show
    uint8_t flagShowLeftSprites; // 0: hide; 1: show
    uint8_t flagShowBackground; // 0: hide; 1: show
    uint8_t flagShowSprites; // 0: hide; 1: show
    uint8_t flagRedTint; // 0: normal; 1: emphasized
    uint8_t flagGreenTint; // 0: normal; 1: emphasized
    uint8_t flagBlueTint; // 0: normal; 1: emphasized

    // $2002 PPUSTATUS
    uint8_t flagSpriteZeroHit;
    uint8_t flagSpriteOverflow;

    // $2003 OAMADDR
    uint8_t oamAddress;

    // $2007 PPUDATA
    uint8_t bufferedData; // for buffered reads

    uint32_t _screenbuffer[256 * 261];

    const uint32_t Palette[64] = {
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

#endif //PNES_PPUNEW_H
