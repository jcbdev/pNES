//
// Created by James Barker on 08/01/2018.
//

#include <cstring>
#include "Ppu.h"
#include "Cpu.h"
#include "Memory.h"
#include "../Rom/Cart.h"
#include "../Helpers/Logger.h"

IPpu::IPpu(ISystem* system){
    _system = system;
}

Ppu::Ppu(ISystem* system) : IPpu::IPpu(system) {

}

void Ppu::Reset() {
    render = false;
    clocks = 1;
    frameToggle = 0;
    //_scanline = 0;
    buffer = 0x00;
    _latch = 0;

    vaddr = 0x0000;
    tileAddr = 0x0000;
    xaddr = 0x00;

    //$2000
    _nmiEnable = false;
    _masterSlave = 0;
    _spriteHeightMode = 0;
    _bgTileSelect = 0x0000;
    _spriteTileSelect = 0x0000;
    _incrementMode = 1;

    //$2001
    _bgrEmphasis = 0;
    _spriteEnable = false;
    _bgEnable = false;
    _spriteLColEnable = false;
    _bgLColEnable = false;
    _grayscale = false;

    //$2002
    _vblank = false;
    _spriteZeroHit = false;
    _spriteOverflow = false;

    //$2003
    _oamAddr = 0x00;

    memset(_screenbuffer, 0, sizeof _screenbuffer);

    memset(_ciram, 0, sizeof _ciram);
    memset(_cgram, 0, sizeof _cgram);
    memset(_oam, 0, sizeof _oam);
    _initPalette();

    scanline = 51;
    dot = 340-65;
}

void Ppu::_initPalette() {
    static const unsigned palette_[] = {
            0x7c7c7c, 0x0000fc, 0x0000bc, 0x4428bc,
            0x940084, 0xa80020, 0xa81000, 0x881400,
            0x503000, 0x007800, 0x006800, 0x005800,
            0x004058, 0x000000, 0x000000, 0x000000,
            0xbcbcbc, 0x0078f8, 0x0058f8, 0x6844fc,
            0xd800cc, 0xe40058, 0xf83800, 0xe45c10,
            0xac7c00, 0x00b800, 0x00a800, 0x00a844,
            0x008888, 0x000000, 0x000000, 0x000000,
            0xf8f8f8, 0x3cbcfc, 0x6888fc, 0x9878f8,
            0xf878f8, 0xf85898, 0xf87858, 0xfca044,
            0xf8b800, 0xb8f818, 0x58d854, 0x58f898,
            0x00e8d8, 0x787878, 0x000000, 0x000000,
            0xfcfcfc, 0xa4e4fc, 0xb8b8b8, 0xd8d8f8,
            0xf8b8f8, 0xf8a4c0, 0xf0d0b0, 0xfce0a8,
            0xf8d878, 0xd8f878, 0xb8f8b8, 0xb8f8d8,
            0x00fcfc, 0xf8d8f8, 0x000000, 0x000000,
    };

    memcpy(_palette, palette_, sizeof(palette_));

    for (unsigned e = 1; e < 8; e++) {
        static const double rfactor[8] = { 1.000, 1.239, 0.794, 1.019, 0.905, 1.023, 0.741, 0.750 };
        static const double gfactor[8] = { 1.000, 0.915, 1.086, 0.980, 1.026, 0.908, 0.987, 0.750 };
        static const double bfactor[8] = { 1.000, 0.743, 0.882, 0.653, 1.277, 0.979, 0.101, 0.750 };
        for (unsigned n = 0; n < 64; n++) {
            uint8_t r = _palette[n][0] >> 16, g = _palette[n][1] >> 8, b = _palette[n][2] >> 0;
            r = _clamp((unsigned)(r * rfactor[e]));
            g = _clamp((unsigned)(g * gfactor[e]));
            b = _clamp((unsigned)(b * bfactor[e]));
            _palette[e * 64 + n][0] = r;
            _palette[e * 64 + n][1] = g;
            _palette[e * 64 + n][2] = b;
        }
    }
}

uint8_t Ppu::_clamp(unsigned x) {
    x = x>255 ? 255 : x;
    return x<0 ? 0 : x;
}

uint8_t Ppu::CiramRead(uint16_t addr){
    return _ciram[addr & 0x07ff];
}

void Ppu::CiramWrite(uint16_t addr, uint8_t data){
    _ciram[addr & 0x07ff] = data;
}

uint8_t Ppu::CgramRead(uint16_t addr){
    if((addr & 0x13) == 0x10) addr &= ~0x10;
    uint8_t data = _cgram[addr & 0x1f];
    if(_grayscale) data &= 0x30;
    return data;
}

void Ppu::CgramWrite(uint16_t addr, uint8_t data){
    if((addr & 0x13) == 0x10) addr &= ~0x10;
    _cgram[addr & 0x1f] = data;
}

bool Ppu::_rasterEnable() {
    return ((_bgEnable || _spriteEnable) && scanline != 261);
}

uint8_t Ppu::_spriteHeight() {
    return _spriteHeightMode == 0 ? 8 : 16;
}

uint8_t Ppu::_scrollX() {
    return ((vaddr & 0x1f) << 3) | xaddr;
}

uint8_t Ppu::_scrollY() {
    return (((vaddr >> 5) & 0x1f) << 3) | ((vaddr >> 12) & 7);
}

void Ppu::_scrollXIncrement() {
    if(_rasterEnable() == false) return;
    vaddr = (vaddr & 0x7fe0) | ((vaddr + 0x0001) & 0x001f);
    if((vaddr & 0x001f) == 0x0000) {
        vaddr ^= 0x0400;
    }
}

void Ppu::_scrollYIncrement() {
    if(_rasterEnable() == false) return;
    vaddr = (vaddr & 0x0fff) | ((vaddr + 0x1000) & 0x7000);
    if((vaddr & 0x7000) == 0x0000) {
        vaddr = (vaddr & 0x7c1f) | ((vaddr + 0x0020) & 0x03e0);
        if((vaddr & 0x03e0) == 0x03c0) {  //0x03c0 == 30 << 5; 30 * 8 = 240
            vaddr &= 0x7c1f;
            vaddr ^= 0x0800;
        }
    }
}

void Ppu::_scanlineIncrement() {
    if(++scanline == 262) {
        scanline = 0;
        _frameEdge();
    }
    _scanlineEdge();
}

uint8_t Ppu::Read(uint16_t addr){
    uint8_t value = 0x00;

    switch(addr & 7) {
        case 2:  //PPUSTATUS
            value |= _vblank << 7;
            value |= _spriteZeroHit << 6;
            value |= _spriteOverflow << 5;
            value |= _mdr & 0x1f;
            _vblank = 0;
            _system->cpu->Nmi(false);
            //status.address_latch = 0;
            break;
        case 4:  //OAMDATA
            value = _oam[_oamAddr];
            if((_oamAddr & 3) == 3) value &= 0xe3;
            break;
        case 7:  //PPUDATA
            if(_rasterEnable() && (scanline <= 240 || scanline == 261)) return 0x00;

            addr = vaddr & 0x3fff;
            if(addr <= 0x1fff) {
                value = buffer;
                buffer = _system->cart->ChrRead(addr);
            } else if(addr <= 0x3eff) {
                value = buffer;
                buffer = _system->cart->ChrRead(addr);
            } else if(addr <= 0x3fff) {
                value = CgramRead(addr);
                buffer = _system->cart->ChrRead(addr);
            }
            vaddr += _incrementMode;
            break;
    }

    return value;
}

void Ppu::Write(uint16_t addr, uint8_t data){
    _mdr = data;

    switch(addr & 7) {
        case 0: //PPUCTRL
            _nmiEnable = data & 0x80;
            _system->cpu->Nmi(_nmiEnable && _vblank);
            _masterSlave = data & 0x40;
            _spriteHeightMode = data & 0x20;
            _bgTileSelect = (data & 0x10) ? 0x1000 : 0x0000;
            _spriteTileSelect = (data & 0x08) ? 0x1000 : 0x0000;
            _incrementMode = (data & 0x04) ? 32 : 1;
            tileAddr = (tileAddr & 0x73ff) | ((data & 0x03) << 10);
            return;
        case 1: //PPUMASK
            _bgrEmphasis = data >> 5;
            _spriteEnable = data & 0x10;
            _bgEnable = data & 0x08;
            _spriteLColEnable = data & 0x04;
            _bgLColEnable = data & 0x02;
            _grayscale = data & 0x01;
            return;
        case 2: //PPUSTATUS
            return;
        case 3: //OAMADDR
            _oamAddr = data;
            return;
        case 4: //OAMDATA
            _oam[_oamAddr++] = data;
            return;
        case 5: //PPUSCROLL
            if(_latch == 0) {
                xaddr = data & 0x07;
                tileAddr = (tileAddr & 0x7fe0) | (data >> 3);
            } else {
                tileAddr = (tileAddr & 0x0c1f) | ((data & 0x07) << 12) | ((data >> 3) << 5);
            }
            _latch ^= 1;
            return;
        case 6: //PPUADDR
            if(_latch == 0) {
                tileAddr = (tileAddr & 0x00ff) | ((data & 0x3f) << 8);
            } else {
                tileAddr = (tileAddr & 0x7f00) | data;
                vaddr = tileAddr;
            }
            _latch ^= 1;
            return;
        case 7: //PPUDATA
            if(_rasterEnable() && (scanline <= 240 || scanline == 261)) return;

            addr = vaddr & 0x3fff;
            if(addr <= 0x1fff) {
                _system->cart->ChrWrite(addr, data);
            } else if(addr <= 0x3eff) {
                _system->cart->ChrWrite(addr, data);
            } else if(addr <= 0x3fff) {
                CgramWrite(addr, data);
            }
            vaddr += _incrementMode;
            return;
    }
}

void Ppu::WriteDMA(uint8_t value) {
        for (uint16_t n = 0; n < 256; n++) {
            uint8_t data = _system->mem->Read(((uint16_t)value << 8) + n);
            _addClocks();
            _system->mem->Write(0x2004, data);
        }
}

uint8_t Ppu::PPUCTRL() {
    uint8_t val;
    val = _nmiEnable << 7;
    val |= _masterSlave << 6;
    val |= _spriteHeightMode << 5;
    val |= (_bgTileSelect == 0x1000 ? 1 : 0) << 4;
    val |= (_spriteTileSelect == 0x1000 ? 1 : 0) << 3;
    val |= (_incrementMode == 32 ? 1 : 0) << 2;
    val |= (tileAddr & 0x03) << 1;
    return val;
}

uint8_t Ppu::PPUMASK() {
    uint8_t val;
    val = _bgrEmphasis << 7;
    val |= _spriteEnable << 4;
    val |= _bgEnable << 3;
    val |= _spriteLColEnable << 2;
    val |= _bgLColEnable << 1;
    val |= _grayscale << 0;
    return val;
}

uint8_t Ppu::PPUSTATUS() {
    uint8_t val;
    val |= _vblank << 7;
    val |= _spriteZeroHit << 6;
    val |= _spriteOverflow << 5;
    val |= _mdr & 0x1f;
    return val;
}

uint8_t Ppu::OAMADDR() {
    return _oamAddr;
}

uint16_t Ppu::V() {
    return vaddr;
}

uint16_t Ppu::X() {
    return xaddr;
}

uint16_t Ppu::T() {
    return tileAddr;
}

bool Ppu::FrameToggle() {
    return frameToggle;
}

uint8_t Ppu::Buffer() {
    return buffer;
}

uint16_t Ppu::Dot() {
    return dot;
}

int16_t Ppu::Scanline() {
    return scanline;
}

uint8_t Ppu::ChrLoad(uint16_t addr){
    if(_rasterEnable() == false)
        return 0x00;
    return _system->cart->ChrRead(addr);
}

void Ppu::_frameEdge() {
    frameToggle ^= 1;
    render = true;
    _totalCycles=0;
}

void Ppu::_scanlineEdge() {
    if(scanline == 241) {
        _vblank = 1;
        if(_nmiEnable) _system->cpu->Nmi(1);
    }
    if(scanline == 261) {
        _vblank = 0;
        _system->cpu->Nmi(0);
        _spriteZeroHit = 0;
        _spriteOverflow = 0;
    }

    dot = 0;

    raster.iterator = 0;
    raster.counter = 0;

    for(unsigned n = 0; n < 8; n++) {
        raster.soam[n].id   = 64;
        raster.soam[n].y    = 0xff;
        raster.soam[n].tile = 0xff;
        raster.soam[n].attributes = 0xff;
        raster.soam[n].x    = 0xff;
        raster.soam[n].tileLo = 0;
        raster.soam[n].tileHi = 0;
    }
}

void Ppu::_rasterPixel(unsigned x) {
    //uint16_t *pixel = _screenbuffer + (_scanline * 256);


    uint16_t mask = 0x8000 >> (xaddr + x);
    uint16_t palette = 0, object_palette = 0;
    bool object_priority = 0;
    palette |= (raster.tileLo & mask) ? 1 : 0;
    palette |= (raster.tileHi & mask) ? 2 : 0;
    if(palette) {
        uint16_t attr = raster.attribute;
        if(mask >= 256) attr >>= 2;
        palette |= (attr & 3) << 2;
    }

    if(_bgEnable == false) palette = 0;
    if(_bgLColEnable == false && dot < 8) palette = 0;

    for(signed sprite = 7; sprite >= 0; sprite--) {
        if(_spriteLColEnable == false && dot < 8) continue;
        if(raster.oam[sprite].id == 64) continue;

        uint16_t spritex = dot - raster.oam[sprite].x;
        if(spritex >= 8) continue;

        if(raster.oam[sprite].attributes & 0x40) spritex ^= 7;
        uint16_t mask = 0x80 >> spritex;
        uint16_t sprite_palette = 0;
        sprite_palette |= (raster.oam[sprite].tileLo & mask) ? 1 : 0;
        sprite_palette |= (raster.oam[sprite].tileHi & mask) ? 2 : 0;
        if(sprite_palette == 0) continue;

        if(raster.oam[sprite].id == 0 && palette)
            _spriteZeroHit = 1;
        sprite_palette |= (raster.oam[sprite].attributes & 3) << 2;

        object_priority = raster.oam[sprite].attributes & 0x20;
        object_palette = 16 + sprite_palette;
    }

    if(object_palette) {
        if(palette == 0 || object_priority == 0) palette = object_palette;
    }

    if(_rasterEnable() == false) palette = 0;
//    _screenbuffer[_dot + (_scanline * 256)] = 0xFF000000;
//    _screenbuffer[_dot + (_scanline * 256)] |= _palette[(_bgrEmphasis << 6) | CgramRead(palette)][0] << 16;
//    _screenbuffer[_dot + (_scanline * 256)] |= _palette[(_bgrEmphasis << 6) | CgramRead(palette)][1] << 8;
//    _screenbuffer[_dot + (_scanline * 256)] |= _palette[(_bgrEmphasis << 6) | CgramRead(palette)][2];
    _screenbuffer[dot + (scanline * 256)] = 0xFF000000 + paletteRGB[(_bgrEmphasis << 6) | CgramRead(palette)];
}

void Ppu::_rasterSprite() {
    if(_spriteEnable == false) return;

    unsigned n = raster.iterator++;
    signed ly = (scanline == 261 ? -1 : scanline);
    unsigned y = ly - _oam[(n * 4) + 0];

    if(y >= _spriteHeight()) return;
    if(raster.counter == 8) {
        _spriteOverflow = 1;
        return;
    }

    raster.soam[raster.counter].id   = n;
    raster.soam[raster.counter].y    = _oam[(n * 4) + 0];
    raster.soam[raster.counter].tile = _oam[(n * 4) + 1];
    raster.soam[raster.counter].attributes = _oam[(n * 4) + 2];
    raster.soam[raster.counter].x    = _oam[(n * 4) + 3];
    raster.counter++;
}

void Ppu::_addClocks() {
    clocks += 1;
    _totalCycles++;
}

void Ppu::Step() {
    //_system->logger->Log("Cycle");
    if (scanline == 261) {
        _visibleScanline();
    }
    else if (scanline >= 0 && scanline < 240) _visibleScanline();
    else if (scanline >= 240 && scanline < 261) _verticalBlankingLine();
}

void Ppu::_preRenderScanline() {

}

void Ppu::_visibleScanline() {
    if (dot < 256)
        _visibleDot();
    if (dot >= 256 && dot < 320)
        _fetchSpriteDataForNextScanline();
    if (dot >= 320 && dot < 336)
        _fetchTileDataForNextScanline();
    if (dot >= 336 && dot < 340)
        _fetchNameTable();
    if (dot == 340) _addClocks();

    dot++;
    if (dot == 341)
        _scanlineIncrement();
}

void Ppu::_visibleDot() {
    unsigned tile = dot / 32;
    unsigned pixel = (dot - (tile * 32)) / 4;

    switch (pixel){
        case 0 :
            _nametableLatch = ChrLoad(0x2000 | (vaddr & 0x0fff));
            _tileAddrLatch = _bgTileSelect + (_nametableLatch << 4) + (_scrollY() & 7);
            _rasterPixel(pixel);
            _addClocks();
            break;
        case 1:
            _rasterPixel(pixel);
            _addClocks();
            break;
        case 2:
            _attributeLatch = ChrLoad(0x23c0 | (vaddr & 0x0fc0) | ((_scrollY() >> 5) << 3) | (_scrollX() >> 5));
            if(_scrollY() & 16) _attributeLatch >>= 4;
            if(_scrollX() & 16) _attributeLatch >>= 2;
            _rasterPixel(pixel);
            _addClocks();
            break;
        case 3:
            _scrollXIncrement();
            if(tile == 31) _scrollYIncrement();
            _rasterPixel(pixel);
            _rasterSprite();
            _addClocks();
            break;
        case 4:
            _tileLoLatch = ChrLoad(_tileAddrLatch + 0);
            _rasterPixel(pixel);
            _addClocks();
            break;
        case 5:
            _rasterPixel(pixel);
            _addClocks();
            break;
        case 6:
            _tileHiLatch = ChrLoad(_tileAddrLatch + 8);
            _rasterPixel(pixel);
            _addClocks();
            break;
        case 7:
            _rasterPixel(7);
            _rasterSprite();
            _addClocks();

            raster.nametable = (raster.nametable << 8) | _nametableLatch;
            raster.attribute = (raster.attribute << 2) | (_attributeLatch & 3);
            raster.tileLo = (raster.tileLo << 8) | _tileLoLatch;
            raster.tileHi = (raster.tileHi << 8) | _tileHiLatch;
            _nametableLatch = 0;
            _attributeLatch = 0;
            _tileLoLatch = 0;
            _tileHiLatch = 0;
            break;
    }
}

void Ppu::_fetchSpriteDataForNextScanline() {
    if (dot == 256) {
        for(unsigned n = 0; n < 8; n++) raster.oam[n] = raster.soam[n];
    }

    unsigned sprite = (dot - 256) / 8;
    unsigned spriteOffset = dot - (sprite * 8) - 256;

    switch (spriteOffset){
        case 0:
            _nametableLatch = ChrLoad(0x2000 | (vaddr & 0x0fff));
            _addClocks();
            break;
        case 1:
            if(_rasterEnable() && sprite == 0) vaddr = (vaddr & 0x7be0) | (tileAddr & 0x041f);
            _addClocks();
            break;
        case 2:
            _attributeLatch = ChrLoad(0x23c0 | (vaddr & 0x0fc0) | ((_scrollY() >> 5) << 3) | (_scrollX() >> 5));
            _tileAddrLatch = (_spriteHeight() == 8)
                                ? _spriteTileSelect + raster.oam[sprite].tile * 16
                                : ((raster.oam[sprite].tile & ~1) * 16) + ((raster.oam[sprite].tile & 1) * 0x1000);
            _addClocks();
            break;
        case 3:
            _addClocks();
            break;
        case 4: {
            unsigned spritey = (scanline - raster.oam[sprite].y) & (_spriteHeight() - 1);
            if (raster.oam[sprite].attributes & 0x80) spritey ^= (_spriteHeight() - 1);
            _tileAddrLatch += spritey + (spritey & 8);
            raster.oam[sprite].tileLo = ChrLoad(_tileAddrLatch + 0);
            _addClocks();
            break;
            }
        case 5:
            _addClocks();
            break;
        case 6:
            raster.oam[sprite].tileHi = ChrLoad(_tileAddrLatch + 8);
            _addClocks();
            break;
        case 7:
            _addClocks();
            if(_rasterEnable() && sprite == 6 && scanline == 261) vaddr = tileAddr;
            _nametableLatch = 0;
            _attributeLatch = 0;
            _tileLoLatch = 0;
            _tileHiLatch = 0;
            break;
    }
}

void Ppu::_fetchTileDataForNextScanline() {
    unsigned tile = (dot - 320) / 8;
    unsigned tileOffset = dot - (tile * 8) - 320;

    switch(tileOffset) {
        case 0:
            _nametableLatch = ChrLoad(0x2000 | (vaddr & 0x0fff));
            _tileAddrLatch = _bgTileSelect + (_nametableLatch << 4) + (_scrollY() & 7);
            _addClocks();
            break;
        case 1:
            _addClocks();
            break;
        case 2:
            _attributeLatch = ChrLoad(0x23c0 | (vaddr & 0x0fc0) | ((_scrollY() >> 5) << 3) | (_scrollX() >> 5));
            if(_scrollY() & 16) _attributeLatch >>= 4;
            if(_scrollX() & 16) _attributeLatch >>= 2;
            _addClocks();
            break;
        case 3:
            _scrollXIncrement();
            _addClocks();
            break;
        case 4:
            _tileLoLatch = ChrLoad(_tileAddrLatch + 0);
            _addClocks();
            break;
        case 5:
            _addClocks();
            break;
        case 6:
            _tileHiLatch = ChrLoad(_tileAddrLatch + 8);
            _addClocks();
            break;
        case 7:
            _addClocks();

            raster.nametable = (raster.nametable << 8) | _nametableLatch;
            raster.attribute = (raster.attribute << 2) | (_attributeLatch & 3);
            raster.tileLo = (raster.tileLo << 8) | _tileLoLatch;
            raster.tileHi = (raster.tileHi << 8) | _tileHiLatch;
            _nametableLatch = 0;
            _attributeLatch = 0;
            _tileLoLatch = 0;
            _tileHiLatch = 0;
            break;
    }
}

void Ppu::_fetchNameTable() {
    switch(dot){
        case 336:
            ChrLoad(0x2000 | (vaddr & 0x0fff));
            _addClocks();
            break;
        case 337:
            _addClocks();
            break;
        case 338:
            ChrLoad(0x2000 | (vaddr & 0x0fff));
            _addClocks();
            break;
        case 339:
            _addClocks();
            break;
    }
}

void Ppu::_verticalBlankingLine() {
    if (dot < 340) {
        _addClocks();
        dot++;
    }
    else {
        if(_rasterEnable() == false || frameToggle != 1 || scanline != 240) {
            _addClocks();
            dot++;
            return _scanlineIncrement();
        }
        dot++;
        _scanlineIncrement();
        if (frameToggle) return Step();
    }

}

uint32_t* Ppu::ScreenBuffer() {
    render = false;

    return (uint32_t *)(_screenbuffer);
}
