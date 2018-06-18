//
// Created by James Barker on 06/04/2018.
//

#include "PpuNew.h"
#include "Memory.h"
#include "Cpu.h"
#include "../Rom/Cart.h"
#include "../Helpers/Logger.h"
#include "Debug.h"


PpuNew::PpuNew(ISystem *system) : IPpu(system) {

}

uint8_t PpuNew::_readPalette(uint16_t address) {
    if (address >= 16 && (address%4) == 0) {
        address -= 16;
    }
    _trace("read palette: " + _tohex(address) + " -> " + _tohex(paletteData[address]));
    return paletteData[address];
}

void PpuNew::_writePalette(uint16_t address, uint8_t value) {
    if (address >= 16 && (address%4) == 0) {
        address -= 16;
    }
    _trace("write palette: " + _tohex(address) + " <- " + _tohex(value));
    paletteData[address] = value;
}

uint8_t PpuNew::ReadRegister(uint16_t addr) {
    _trace("read register: " + _tohex(addr));
    switch (addr){
        case 0x2002:
            return _readStatus();
        case 0x2004:
            return _readOAMData();
        case 0x2007:
            return _readData();
        default:
            return 0;
    }
}

void PpuNew::WriteRegister(uint16_t addr, uint8_t data) {
    _trace("write register: " + _tohex(addr) + ", " + _tohex(data));
    reg = data;
    switch (addr){
        case 0x2000:
            _writeControl(data);
            break;
        case 0x2001:
            _writeMask(data);
            break;
        case 0x2003:
            _writeOAMAddress(data);
            break;
        case 0x2004:
            _writeOAMData(data);
            break;
        case 0x2005:
            _writeScroll(data);
            break;
        case 0x2006:
            _writeAddress(data);
            break;
        case 0x2007:
            _writeData(data);
            break;
        case 0x4014:
            WriteDMA(data);
            break;
    }
}

// $2000: PPUCTRL
void PpuNew::_writeControl(uint8_t value) {
    _trace("write control: " + _tohex(value));
    flagNameTable = (value >> 0) & 3;
    flagIncrement = (value >> 2) & 1;
    flagSpriteTable = (value >> 3) & 1;
    flagBackgroundTable = (value >> 4) & 1;
    flagSpriteSize = (value >> 5) & 1;
    flagMasterSlave = (value >> 6) & 1;
    nmiOutput = (value>>7)&1 == 1;
    _nmiChange();
    // t: ....BA.. ........ = d: ......BA
    //t = (t & 0xF3FF) | (((uint16_t)(value) & 0x03) << 10);
    t = (t & 0x73FF) | (((uint16_t)(value) & 0x03) << 10);
}

// $2001: PPUMASK
void PpuNew::_writeMask(uint8_t value) {
    _trace("write mask: " + _tohex(value));
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
    _trace("read status: -> " + _tohex(result));
    return result;
}

// $2003: OAMADDR
void PpuNew::_writeOAMAddress(uint8_t value) {
    _trace("write oam address: " + _tohex(value));
    oamAddress = value;
}

// $2004: OAMDATA (read)
uint8_t PpuNew::_readOAMData() {
    _trace("read oam data: -> " + _tohex(oamData[oamAddress]));
    return oamData[oamAddress];
}

// $2004: OAMDATA (write)
void PpuNew::_writeOAMData(uint8_t value) {
    _trace("write oam data: " + _tohex(value));
    oamData[oamAddress] = value;
    oamAddress++;
}

// $2005: PPUSCROLL
void PpuNew::_writeScroll(uint8_t value) {
    _trace("write scroll: " + _tohex(value));
    if (w == 0) {
        // t: ........ ...HGFED = d: HGFED...
        // x:               CBA = d: .....CBA
        // w:                   = 1
        t = (t & 0xFFE0) | ((uint16_t)(value) >> 3);
        //t = (t & 0x7FE0) | ((uint16_t)(value) >> 3);
        x = value & 0x07;
        w = 1;
    } else {
        // t: .CBA..HG FED..... = d: HGFEDCBA
        // w:                   = 0
        t = (t & 0x8FFF) | (((uint16_t)(value) & 0x07) << 12);
        t = (t & 0xFC1F) | (((uint16_t)(value) & 0xF8) << 2);
        //t = (t & 0x0C1F) | (((uint16_t)(value) & 0x07) << 12) | ((value >> 3) << 5);
        w = 0;
    }
}

// $2006: PPUADDR
void PpuNew::_writeAddress(uint8_t value) {
    _trace("write address: " + _tohex(value));
    //return;
    if (w == 0) {
        // t: ..FEDCBA ........ = d: ..FEDCBA
        // t: .X...... ........ = 0
        // w:                   = 1
        t = (t & 0x80FF) | (((uint16_t)(value) & 0x3F) << 8);
        //t = (t & 0x00FF) | (((uint16_t)(value) & 0x3F) << 8);
        w = 1;
    } else {
        // t: ........ HGFEDCBA = d: HGFEDCBA
        // v                    = t
        // w:                   = 0
        t = (t & 0xFF00) | (uint16_t)(value);
        //t = (t & 0x7F00) | (uint16_t)(value);
        v = t;
        w = 0;
    }
}

// $2007: PPUDATA (read)
uint8_t PpuNew::_readData() {
    if ((flagShowBackground || flagShowSprites) && (scanline <= 240 || scanline == 261)) return 0x00;
    uint8_t value = Read(v);

    // emulate buffered reads
    if ((v%0x4000) < 0x3F00) {
        uint8_t buffered = bufferedData;
        bufferedData = value;
        value = buffered;
    } else {
        bufferedData = _system->ppu->Read(v - 0x1000);
    }

    // increment address
    if (flagIncrement == 0) {
        v += 1;
    } else {
        v += 32;
    }
    _trace("read data: -> " + _tohex(value));
    return value;
}

// $2007: PPUDATA (write)
void PpuNew::_writeData(uint8_t value) {
    if ((flagShowBackground || flagShowSprites) && (scanline <= 240 || scanline == 261)) return;
    _trace("write data: " + _tohex(value));
    //_system->mem->Write(v, value);
    Write(v, value);

    if (flagIncrement == 0) {
        v += 1;
    } else {
        v += 32;
    }
}

// $4014: OAMDMA
void PpuNew::WriteDMA(uint8_t value){
    _trace("write dma: " + _tohex(value));
    uint16_t address = (uint16_t)(value) << 8;
    for (int i = 0; i < 256; i++) {
        oamData[oamAddress] = _system->mem->Read(address);
        //oamData[oamAddress] = 0;
        oamAddress++;
        address++;
    }
    _system->cpu->clocks += 513 * 3;
    if ((_system->cpu->totalClocks%2) == 1) {
        _system->cpu->clocks = _system->cpu->clocks + 3;
    }
}

// NTSC Timing Helper Functions
void PpuNew::_incrementX() {
    _trace("increment x");
    // increment hori(v)
    // if coarse X == 31
    if ((v&0x001F) == 31) {
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
    _trace("increment y");
    // increment vert(v)
    // if fine Y < 7
    if ((v&0x7000) != 0x7000) {
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
        v = (v & ~0x03E0) | (y << 5);
    }
}

void PpuNew::_copyX() {
    _trace("copy x");
    // hori(v) = hori(t)
    // v: .....F.. ...EDCBA = t: .....F.. ...EDCBA
    v = (v & 0xFBE0) | (t & 0x041F);
}

void PpuNew::_copyY() {
    _trace("copy y");
    // vert(v) = vert(t)
    // v: .IHGF.ED CBA..... = t: .IHGF.ED CBA.....
    v = (v & 0x841F) | (t & 0x7BE0);
}

void PpuNew::_nmiChange() {
    bool nmi = nmiOutput && nmiOccurred;
    if (nmi && !nmiPrevious) {
        _trace("nmi change");
        // TODO: this fixes some games but the delay shouldn't have to be so
        // long, so the timings are off somewhere
        nmiDelay = 0;
    }
    nmiPrevious = nmi;
}


void PpuNew::_setVerticalBlank() {
    _trace("set vertical blank");
    nmiOccurred = true;
    _nmiChange();
}

void PpuNew::_clearVerticalBlank() {
    _trace("clear vertical blank");
    nmiOccurred = false;
    _nmiChange();
}

void PpuNew::_fetchNameTableByte() {
    //nameTableByte = 0xFF;
    //return;
    uint16_t address = 0x2000 | (v & 0x0FFF);
    uint8_t val = Read(address);
    _trace("fetch name table byte: " + _tohex(address) + " -> " + _tohex(val));
    nameTableByte = val;
}

void PpuNew::_fetchAttributeTableByte() {
    //attributeTableByte = 0x00;
    //return;
    uint16_t address = 0x23C0 | (v & 0x0C00) | ((v >> 4) & 0x38) | ((v >> 2) & 0x07);
    uint8_t shift = ((v >> 4) & 4) | (v & 2);
    uint8_t val = ((Read(address) >> shift) & 3) << 2;
    _trace("fetch attribute byte: " + _tohex(address) + " -> " + _tohex(val));
    attributeTableByte = val;
}

void PpuNew::_fetchLowTileByte() {
    //lowTileByte = 0x00;
    //return;
    uint16_t fineY = (v >> 12) & 7;
    uint16_t address = 0x1000*((uint16_t)(flagBackgroundTable)) + ((uint16_t)(nameTableByte)*16) + fineY;
    uint8_t val = Read(address);
    _trace("fetch low tile byte: " + _tohex(address) + " -> " + _tohex(val));
    lowTileByte = val;
}

void PpuNew::_fetchHighTileByte() {
    uint16_t fineY = (v >> 12) & 7;
    uint16_t address = 0x1000*(uint16_t)(flagBackgroundTable) + ((uint16_t)(nameTableByte)*16) + fineY;
    uint8_t val = Read(address + 8);
    _trace("fetch high tile byte: " + _tohex((uint16_t)(address + 8)) + " -> " + _tohex(val));
    highTileByte = val;
}

void PpuNew::_storeTileData() {

    uint32_t data = 0;
    for (int i = 0; i < 8; i++) {
        uint8_t p1 = (lowTileByte & 0x80) >> 7;
        uint8_t p2 = (highTileByte & 0x80) >> 6;
        lowTileByte <<= 1;
        highTileByte <<= 1;
        data <<= 4;
        data |= (uint32_t)(attributeTableByte | p1 | p2);
    }
    _trace("store tile data: <- " + _tohex(data));
    tileData |= (uint64_t)(data);
}

uint32_t PpuNew::_fetchTileData() {
    _trace("fetch tile data: -> " + _tohex((uint32_t)(tileData >> 32)));
    return (uint32_t)(tileData >> 32);
}

uint8_t PpuNew::_backgroundPixel() {
    //return 3;
    if (flagShowBackground == 0) {
        return 0;
    }
    uint64_t data = _fetchTileData() >> ((7 - x) * 4);
    return (uint8_t)(data & 0x0F);
}

spr PpuNew::_spritePixel() {
    if (flagShowSprites == 0) {
        return { 0, 0 };
    }
    for (int i = 0; i < spriteCount; i++) {
        int offset = (cycle - 1) - int(spritePositions[i]);
        if (offset < 0 || offset > 7) {
            continue;
        }
        offset = 7 - offset;
        uint8_t color = (uint8_t)((spritePatterns[i] >> (uint8_t)(offset*4)) & 0x0F);
        if ((color%4) == 0) {
            continue;
        }
        return { (uint8_t)(i), color };
    }
    return { 0, 0 };
}

void PpuNew::_renderPixel() {
    _trace("render pixel");
    int x = cycle - 1;
    int y = scanline;
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
    bool b = (background%4) != 0;
    bool s = (sprite%4) != 0;
    uint8_t color = 0;
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

//    _testbuffer[(x*3) + (y*256*3)] = color*4;
//    _testbuffer[1 + (x*3) + (y*256*3)] = color*4;
//    _testbuffer[2 + (x*3) + (y*256*3)] = color*4;
    //color = 3;
    uint32_t c = Palette[_readPalette((uint16_t)(color))%64];
    //ppu.back.SetRGBA(x, y, c)
    _screenbuffer[x + (y * 256)] =  (c << 8) + 0xFF;
    _testbuffer[(x*3) + (y*256*3)] = (uint8_t)(c >> 16) & 0xFF;
    _testbuffer[1 + (x*3) + (y*256*3)] = (uint8_t)(c >> 8) & 0xFF;
    _testbuffer[2 + (x*3) + (y*256*3)] = (uint8_t)(c) & 0xFF;
}

uint32_t PpuNew::_fetchSpritePattern(int i, int row) {
    uint8_t tile = oamData[i*4+1];
    uint8_t attributes = oamData[i*4+2];
    uint16_t address= 0;
    if (flagSpriteSize == 0) {
        if ((attributes&0x80) == 0x80) {
            row = 7 - row;
        }
        uint8_t table = flagSpriteTable;
        address = 0x1000*(uint16_t)(flagSpriteTable) + (uint16_t)(tile)*16 + (uint16_t)(row);
    } else {
        if ((attributes&0x80) == 0x80) {
            row = 15 - row;
        }
        uint8_t table = tile & 1;
        tile &= 0xFE;
        if (row > 7) {
            tile++;
            row -= 8;
        }
        address = 0x1000*(uint16_t)(table) + (uint16_t)(tile)*16 + (uint16_t)(row);
    }
    uint8_t a = (attributes & 3) << 2;
    uint8_t lowTileByte = Read(address);
    uint8_t highTileByte = Read(address + 8);
    uint32_t data = 0;
    for (int i = 0; i < 8; i++) {
        uint8_t p1, p2;
        if ((attributes&0x40) == 0x40) {
            p1 = (lowTileByte & 1) << 0;
            p2 = (highTileByte & 1) << 1;
            lowTileByte >>= 1;
            highTileByte >>= 1;
        } else {
            p1 = (lowTileByte & 0x80) >> 7;
            p2 = (highTileByte & 0x80) >> 6;
            lowTileByte <<= 1;
            highTileByte <<= 1;
        }
        data <<= 4;
        data |= (uint32_t)(a | p1 | p2);
    }
    _trace("fetch sprite pattern: " + std::to_string(i) + ", " + std::to_string(row) + " -> " + _tohex(data));
    return data;
}

void PpuNew::_evaluateSprites() {
    _trace("evaluate sprites");
    int h;
    if (flagSpriteSize == 0) {
        h = 8;
    } else {
        h = 16;
    }
    int count = 0;
    for (int i = 0; i < 64; i++) {
        uint8_t y = oamData[i*4+0];
        uint8_t a = oamData[i*4+2];
        uint8_t x = oamData[i*4+3];
        int row = scanline - (int)(y);
        if (row < 0 || row >= h) {
            continue;
        }
        if (count < 8) {
            spritePatterns[count] = _fetchSpritePattern(i, row);
            spritePositions[count] = x;
            spritePriorities[count] = (a >> 5) & 1;
            spriteIndexes[count] = (uint8_t)(i);
        }
        count++;
    }
    if (count > 8) {
        count = 8;
        flagSpriteOverflow = 1;
    }
    spriteCount = count;
}

void PpuNew::_checkBreak() {
    if (scanlinesToRunFor > 0) {
        scanlinesToRunFor--;
        if (scanlinesToRunFor == 0)
            breakOnNextScanline = true;
    }
    if (framesToRunFor > 0) {
        framesToRunFor--;
        if (framesToRunFor == 0)
            breakOnNextScanline = true;
    }
};

void PpuNew::tick() {
    _trace(".", true);
    clocks++;
    //if (nmiDelay > 0) {
    //    nmiDelay--;
        if (nmiDelay == 0 && nmiOutput) {
            _system->cpu->Nmi(nmiOccurred);
        }
    //}

    if (flagShowBackground != 0 || flagShowSprites != 0) {
        if (f == 1 && scanline == 261 && cycle == 339) {
            cycle = 0;
            scanline = 0;
            frame++;
            _checkBreak();
            f ^= 1;
            return;
        }
    }
    cycle++;
    if (cycle > 340) {
        cycle = 0;
        scanline++;
        if (scanlinesToRunFor > 0) {
            scanlinesToRunFor--;
            if (scanlinesToRunFor == 0)
                breakOnNextScanline = true;
        }
        if (scanline > 261) {
            scanline = 0;
            frame++;
            _checkBreak();
            render = true;
            f ^= 1;
        }
    }
}

// Step executes a single PPU cycle
void PpuNew::Step() {

    tick();

    bool renderingEnabled = flagShowBackground != 0 || flagShowSprites != 0;
    bool preLine = scanline == 261;
    bool visibleLine = scanline < 240;
    // bool postLine = ScanLine == 240;
    bool renderLine = preLine || visibleLine;
    bool preFetchCycle = cycle >= 321 && cycle <= 336;
    bool visibleCycle = cycle >= 1 && cycle <= 256;
    bool fetchCycle = preFetchCycle || visibleCycle;

    // background logic
    if (renderingEnabled) {
        if (visibleLine && visibleCycle) {
            _renderPixel();
        }
        if (renderLine && fetchCycle) {
            tileData <<= 4;
            switch (cycle % 8) {
                case 1:
                    _fetchNameTableByte();
                    break;
                case 3:
                    _fetchAttributeTableByte();
                    break;
                case 5:
                    _fetchLowTileByte();
                    break;
                case 7:
                    _fetchHighTileByte();
                    break;
                case 0:
                    _storeTileData();
                    break;
            }
        }
        if (preLine && cycle >= 280 && cycle <= 304) {
            _copyY();
        }
        if (renderLine) {
            if (fetchCycle && (cycle%8) == 0) {
                _incrementX();
            }
            if (cycle == 256) {
                _incrementY();
            }
            if (cycle == 257) {
                _copyX();
            }
        }
    }

    // sprite logic
    if (renderingEnabled) {
        if (cycle == 257) {
            if (visibleLine) {
                _evaluateSprites();
            } else {
                spriteCount = 0;
            }
        }
    }

    // vblank logic
    if (scanline == 241 && cycle == 1) {
        _setVerticalBlank();
    }
    if (preLine && cycle == 1) {
        _clearVerticalBlank();
        flagSpriteZeroHit = 0;
        flagSpriteOverflow = 0;
    }

    if (cycle == 340 && breakOnNextScanline) {
        breakOnNextScanline = false;
        _system->debug->Break();
        Snapshot();
    }
    if (f == 1 && scanline == 261 && cycle == 339 && breakOnNextScanline) {
        breakOnNextScanline = false;
        _system->debug->Break();
        Snapshot();
    }
}

void PpuNew::Reset() {
    cycle = 340;
    scanline = 240;
    frame = 0;
    _writeControl(0);
    _writeMask(0);
    _writeOAMAddress(0);
    breakOnNextScanline = false;
    scanlinesToRunFor = 0;
    framesToRunFor = 0;
}

uint8_t PpuNew::Read(uint16_t addr) {
    uint16_t address = addr % 0x4000;
    _trace("read: " + _tohex(address));

    if (address < 0x2000)
        return _system->cart->Read(address);
    if (address < 0x3F00) {
        return nameTableData[_mirrorAddress(address) % 2048];
    }
    if (address < 0x4000)
        return _readPalette(address % 32);
    _system->logger->Log("Ppu Read: Unknown address");
    return 0;
}

void PpuNew::Write(uint16_t addr, uint8_t data) {
    uint16_t address = addr % 0x4000;
    _trace("write: " + _tohex(address) + " <- " + _tohex(data));
    if (address < 0x2000) {
        //data = 0;
        _system->cart->Write(address, data);
    }
    else if (address < 0x3F00) {
        //data = 0;
        nameTableData[_mirrorAddress(address) % 2048] = data;
        //nameTableData[address%2048] = data;
    }
    else if (address < 0x4000) {
        //data = 1;
        _writePalette(address%32, data);
    }
    else _system->logger->Log("Ppu Write: Unknown address");
}

uint16_t PpuNew::_mirrorAddress(uint16_t addr) {
    uint16_t address = (addr - 0x2000) % 0x1000;
    auto table = address / 0x0400;
    auto offset = address % 0x0400;
    uint16_t retaddr = 0x2000 + Mirror[_system->cart->Header.MirrorMode()][table]*0x0400 + offset;
    _trace("ppu mirror address: " + _tohex(addr) + " -> " + _tohex(retaddr));
    return retaddr;
}

uint8_t PpuNew::PPUCTRL() {
    uint8_t val;
    val = nmiOutput << 7;
    val |= flagMasterSlave << 6;
    val |= flagSpriteSize << 5;
    val |= (flagBackgroundTable == 0x1000 ? 1 : 0) << 4;
    val |= (flagSpriteTable == 0x1000 ? 1 : 0) << 3;
    val |= (flagIncrement == 32 ? 1 : 0) << 2;
    val |= (tileData & 0x03) << 1;
    return val;
}

uint8_t PpuNew::PPUMASK() {
    uint8_t val;
    val = flagRedTint << 7;
    val |= flagShowSprites << 4;
    val |= flagShowBackground << 3;
    val |= flagShowLeftSprites << 2;
    val |= flagShowLeftBackground << 1;
    val |= flagGrayscale << 0;
    return val;
}

uint8_t PpuNew::PPUSTATUS() {
    uint8_t val;
    val |= nmiOccurred << 7;
    val |= flagSpriteZeroHit << 6;
    val |= flagSpriteOverflow << 5;
    val |= bufferedData & 0x1f;
    return val;
}

uint8_t PpuNew::OAMADDR() {
    return oamAddress;
}

uint16_t PpuNew::V() {
    return v;
}

uint16_t PpuNew::X() {
    return x;
}

uint16_t PpuNew::T() {
    return t;
}

bool PpuNew::FrameToggle() {
    return f;
}

uint8_t PpuNew::Buffer() {
    return bufferedData;
}

uint16_t PpuNew::Dot() {
    return cycle;
}

int16_t PpuNew::Scanline() {
    return scanline;
}

uint32_t* PpuNew::ScreenBuffer() {
    render = false;

    return (uint32_t *)(_screenbuffer);
}

uint8_t* PpuNew::TestBuffer() {
    render = false;

    return _testbuffer;
}

void PpuNew::Snapshot() {
    return;
    //FCEUX Format
    std::ofstream snapshotFile;
    snapshotFile.open("ppu-snapshot.ram", std::ios::out | std::ios::binary);
    snapshotFile.write((char*)&nameTableData, sizeof(uint8_t[2048]));
    snapshotFile.write((char*)&paletteData, sizeof(uint8_t[32]));
    snapshotFile.write((char*)&oamData, sizeof(uint8_t[256]));
    uint8_t ppuctrl = PPUCTRL();
    snapshotFile.write((char*)&ppuctrl, sizeof(uint8_t));
    uint8_t ppumask = PPUMASK();
    snapshotFile.write((char*)&ppumask, sizeof(uint8_t));
    uint8_t ppustatus = PPUSTATUS();
    snapshotFile.write((char*)&ppustatus, sizeof(uint8_t));
    snapshotFile.write((char*)&oamAddress, sizeof(uint8_t));
    snapshotFile.write((char*)&x, sizeof(uint8_t));
    snapshotFile.write((char*)&w, sizeof(uint8_t));
    snapshotFile.write((char*)&v, sizeof(uint16_t));
    snapshotFile.write((char*)&t, sizeof(uint16_t));
    snapshotFile.write((char*)&bufferedData, sizeof(uint8_t));
    //Missing LAtch?
    snapshotFile.close();
}

void PpuNew::_trace(std::string log, bool showstats) {
    std::stringstream output;
    if (showstats) {
        output << "    <<< ";
        output << std::setw(3) << cycle << ":" << std::setw(3) << scanline
            << "    v:" << _tohex(v)
            << ".fY:" << _tohex((uint8_t)((v & 0x7fff) >> 12))
            << ".NN:" << _tohex((uint8_t)((v >> 10) & 0x03))
            << ".cY:" << _tohex((uint8_t)((v >> 5) & 0x1F))
            << ".cX:" << _tohex((uint8_t)((v) & 0x1F))
            << ", t:" << _tohex(t)
            << ", fX:" << _tohex((uint8_t)(x))
            << ", NT:" << _tohex(nameTableByte)
            << ", Att:" << _tohex(attributeTableByte)
            << ", Ltb:" << _tohex(lowTileByte)
            << ", Htb:" << _tohex(highTileByte)
            << ", Fbt:" << _tohex(flagBackgroundTable);
        output << " >>>";
    }
    else {
        output << "        * ppu " << log;
    }
    _system->logger->Log(output.str());
}


