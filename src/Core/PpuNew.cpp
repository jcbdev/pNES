//
// Created by James Barker on 06/04/2018.
//

#include "PpuNew.h"

PpuNew::PpuNew(ISystem *system) {

}

uint8_t PpuNew::_readPalette(uint16_t address) {
    if (address >= 16 && address%4 == 0) {
        address -= 16;
    }
    return paletteData[address];
}

void PpuNew::_writePalette(uint16_t address, uint8_t value) {
    if (address >= 16 && address%4 == 0) {
        address -= 16;
    }
    paletteData[address] = value;
}


// $2000: PPUCTRL
void PpuNew::_writeControl(uint8_t value) {
    flagNameTable = (value >> 0) & 3;
    flagIncrement = (value >> 2) & 1;
    flagSpriteTable = (value >> 3) & 1;
    flagBackgroundTable = (value >> 4) & 1;
    flagSpriteSize = (value >> 5) & 1;
    flagMasterSlave = (value >> 6) & 1;
    nmiOutput = (value>>7)&1 == 1;
    _nmiChange();
    // t: ....BA.. ........ = d: ......BA
    t = (t & 0xF3FF) | (((uint16_t)(value) & 0x03) << 10);
}

// $2001: PPUMASK
void PpuNew::_writeMask(uint8_t value) {
    flagGrayscale = (value >> 0) & 1;
    flagShowLeftBackground = (value >> 1) & 1;
    flagShowLeftSprites = (value >> 2) & 1;
    flagShowBackground = (value >> 3) & 1;
    flagShowSprites = (value >> 4) & 1;
    flagRedTint = (value >> 5) & 1;
    flagGreenTint = (value >> 6) & 1;
    flagBlueTint = (value >> 7) & 1;
}

// $2002: PPUSTATUS
uint8_t PpuNew::_readStatus() {
    uint8_t result = reg & 0x1F;
    result |= flagSpriteOverflow << 5;
    result |= flagSpriteZeroHit << 6;
    if (nmiOccurred) {
        result |= 1 << 7;
    }
    nmiOccurred = false;
    _nmiChange();
    // w:                   = 0
    w = 0;
    return result;
}

// $2003: OAMADDR
void PpuNew::_writeOAMAddress(uint8_t value) {
    oamAddress = value;
}

// $2004: OAMDATA (read)
uint8_t PpuNew::_readOAMData() {
    return oamData[oamAddress];
}

// $2004: OAMDATA (write)
void PpuNew::_writeOAMData(uint8_t value) {
    oamData[oamAddress] = value;
    oamAddress++;
}

// $2005: PPUSCROLL
void PpuNew::_writeScroll(uint8_t value) {
    if (w == 0) {
        // t: ........ ...HGFED = d: HGFED...
        // x:               CBA = d: .....CBA
        // w:                   = 1
        t = (t & 0xFFE0) | ((uint16_t)(value) >> 3);
        x = value & 0x07;
        w = 1;
    } else {
        // t: .CBA..HG FED..... = d: HGFEDCBA
        // w:                   = 0
        t = (t & 0x8FFF) | (((uint16_t)(value) & 0x07) << 12);
        t = (t & 0xFC1F) | (((uint16_t)(value) & 0xF8) << 2);
        w = 0;
    }
}

// $2006: PPUADDR
void PpuNew::_writeAddress(uint8_t value) {
    if (w == 0) {
        // t: ..FEDCBA ........ = d: ..FEDCBA
        // t: .X...... ........ = 0
        // w:                   = 1
        t = (t & 0x80FF) | (((uint16_t)(value) & 0x3F) << 8);
        w = 1;
    } else {
        // t: ........ HGFEDCBA = d: HGFEDCBA
        // v                    = t
        // w:                   = 0
        t = (t & 0xFF00) | (uint16_t)(value);
        v = t;
        w = 0;
    }
}

// $2007: PPUDATA (read)
uint8_t PpuNew::_readData() {
    uint8_t value = Read(v);
    // emulate buffered reads
    if (v%0x4000 < 0x3F00) {
        uint8_t buffered = bufferedData;
        bufferedData = value;
        value = buffered;
    } else {
        bufferedData = Read(v - 0x1000);
    }
    // increment address
    if (flagIncrement == 0) {
        v += 1;
    } else {
        v += 32;
    }
    return value;
}

// $2007: PPUDATA (write)
void PpuNew::_writeData(uint8_t value) {
    Write(v, value);
    if (flagIncrement == 0) {
        v += 1;
    } else {
        v += 32;
    }
}

// $4014: OAMDMA
void PpuNew::_writeDMA(uint8_t value) {
    uint16_t address = (uint16_t)(value) << 8;
    for (int i = 0; i < 256; i++) {
        oamData[oamAddress] = cpu.Read(address);
        oamAddress++;
        address++;
    }
    cpu.stall += 513;
    if (cpu.Cycles%2 == 1) {
        cpu.stall++;
    }
}

// NTSC Timing Helper Functions
void PpuNew::_incrementX() {
    // increment hori(v)
    // if coarse X == 31
    if (v&0x001F == 31) {
        // coarse X = 0
        v &= 0xFFE0;
        // switch horizontal nametable
        v ^= 0x0400;
    } else {
        // increment coarse X
        v++;
    }
}

void PpuNew::_incrementY() {
    // increment vert(v)
    // if fine Y < 7
    if (v&0x7000 != 0x7000) {
        // increment fine Y
        v += 0x1000;
    } else {
        // fine Y = 0
        v &= 0x8FFF;
        // let y = coarse Y
        uint16_t y = (v & 0x03E0) >> 5;
        if (y == 29) {
            // coarse Y = 0
            y = 0;
            // switch vertical nametable
            v ^= 0x0800;
        } else if (y == 31) {
            // coarse Y = 0, nametable not switched
            y = 0;
        } else {
            // increment coarse Y
            y++;
        }
        // put coarse Y back into v
        v = (v & 0xFC1F) | (y << 5);
    }
}

void PpuNew::_copyX() {
    // hori(v) = hori(t)
    // v: .....F.. ...EDCBA = t: .....F.. ...EDCBA
    v = (v & 0xFBE0) | (t & 0x041F);
}

void PpuNew::_copyY() {
    // vert(v) = vert(t)
    // v: .IHGF.ED CBA..... = t: .IHGF.ED CBA.....
    v = (v & 0x841F) | (t & 0x7BE0);
}

void PpuNew::_nmiChange() {
    bool nmi = nmiOutput && nmiOccurred;
    if (nmi && !nmiPrevious) {
        // TODO: this fixes some games but the delay shouldn't have to be so
        // long, so the timings are off somewhere
        nmiDelay = 15;
    }
    nmiPrevious = nmi;
}


void PpuNew::_setVerticalBlank() {
    nmiOccurred = true;
    _nmiChange();
}

void PpuNew::_clearVerticalBlank() {
    nmiOccurred = false;
    _nmiChange();
}

void PpuNew::_fetchNameTableByte() {
    uint16_t address = 0x2000 | (v & 0x0FFF);
    nameTableByte = Read(address);
}

void PpuNew::_fetchAttributeTableByte() {
    uint16_t address = 0x23C0 | (v & 0x0C00) | ((v >> 4) & 0x38) | ((v >> 2) & 0x07);
    uint8_t shift = ((v >> 4) & 4) | (v & 2);
    attributeTableByte = ((Read(address) >> shift) & 3) << 2;
}

void PpuNew::_fetchLowTileByte() {
    uint16_t fineY = (v >> 12) & 7;
    uint16_t address = 0x1000*(uint16_t)(flagBackgroundTable) + (uint16_t)(nameTableByte)*16 + fineY;
    lowTileByte = Read(address);
}

void PpuNew::_fetchHighTileByte() {
    uint16_t fineY = (v >> 12) & 7;
    uint16_t address = 0x1000*(uint16_t)(flagBackgroundTable) + (uint16_t)(nameTableByte)*16 + fineY;
    highTileByte = Read(address + 8);
}

void PpuNew::_storeTileData() {
    uint32_t data;
    for (int i = 0; i < 8; i++) {
        uint8_t p1 = (lowTileByte & 0x80) >> 7;
        uint8_t p2 = (highTileByte & 0x80) >> 6;
        lowTileByte <<= 1;
        highTileByte <<= 1;
        data <<= 4;
        data |= (uint32_t)(attributeTableByte | p1 | p2);
    }
    tileData |= (uint64_t)(data);
}

uint32_t PpuNew::_fetchTileData() {
    return (uint32_t)(tileData >> 32);
}

uint8_t PpuNew::_backgroundPixel() {
    if (flagShowBackground == 0) {
        return 0;
    }
    uint16_t data = _fetchTileData() >> ((7 - x) * 4);
    return (uint8_t)(data & 0x0F);
}

spr PpuNew::_spritePixel() {
    if (flagShowSprites == 0) {
        return { 0, 0 };
    }
    for (int i = 0; i < spriteCount; i++) {
        int offset = (Cycle - 1) - int(spritePositions[i]);
        if (offset < 0 || offset > 7) {
            continue;
        }
        offset = 7 - offset;
        uint8_t color = (uint8_t)((spritePatterns[i] >> (uint8_t)(offset*4)) & 0x0F);
        if (color%4 == 0) {
            continue;
        }
        return { (uint8_t)(i), color };
    }
    return { 0, 0 };
}

void PpuNew::_renderPixel() {
    int x = Cycle - 1;
    int y = ScanLine;
    uint8_t background = _backgroundPixel();
    spr spriteData = _spritePixel();
    uint8_t i = spriteData.a;
    uint8_t sprite = spriteData.b;
    if (x < 8 && flagShowLeftBackground == 0) {
        background = 0;
    }
    if (x < 8 && flagShowLeftSprites == 0) {
        sprite = 0;
    }
    bool b = background%4 != 0;
    bool s = sprite%4 != 0;
    uint8_t color;
    if (!b && !s) {
            color = 0;
        } else if (!b && s) {
            color = sprite | 0x10;
        } else if (b && !s) {
            color = background;
        } else {
        if (spriteIndexes[i] == 0 && x < 255) {
            flagSpriteZeroHit = 1;
        }
        if (spritePriorities[i] == 0) {
            color = sprite | 0x10;
        } else {
            color = background;
        }
    }
    uint8_t c = Palette[_readPalette((uint16_t)(color))%64];
    //ppu.back.SetRGBA(x, y, c)
}

uint32_t PpuNew::_fetchSpritePattern(int i, int row) {
    tile := ppu.oamData[i*4+1]
    attributes := ppu.oamData[i*4+2]
    var address uint16
    if ppu.flagSpriteSize == 0 {
    if attributes&0x80 == 0x80 {
    row = 7 - row
    }
    table := ppu.flagSpriteTable
    address = 0x1000*uint16(table) + uint16(tile)*16 + uint16(row)
    } else {
    if attributes&0x80 == 0x80 {
    row = 15 - row
    }
    table := tile & 1
    tile &= 0xFE
    if row > 7 {
    tile++
    row -= 8
    }
    address = 0x1000*uint16(table) + uint16(tile)*16 + uint16(row)
    }
    a := (attributes & 3) << 2
    lowTileByte := ppu.Read(address)
    highTileByte := ppu.Read(address + 8)
    var data uint32
    for i := 0; i < 8; i++ {
    var p1, p2 byte
    if attributes&0x40 == 0x40 {
    p1 = (lowTileByte & 1) << 0
    p2 = (highTileByte & 1) << 1
    lowTileByte >>= 1
    highTileByte >>= 1
    } else {
    p1 = (lowTileByte & 0x80) >> 7
    p2 = (highTileByte & 0x80) >> 6
    lowTileByte <<= 1
    highTileByte <<= 1
    }
    data <<= 4
    data |= uint32(a | p1 | p2)
    }
    return data
}