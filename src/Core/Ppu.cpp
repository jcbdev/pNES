//
// Created by James Barker on 08/01/2018.
//

#include <cstring>
#include "Ppu.h"
#include "Cpu.h"
#include "../Rom/Cart.h"

IPpu::IPpu(ISystem* system){
    _system = system;
}

Ppu::Ppu(ISystem* system) : IPpu::IPpu(system) {

}

void Ppu::Reset() {
    render = false;
    clocks = 1;
    _mbr = 0x00;
    _field = 0;
    _scanline = 0;
    _data = 0x00;
    _latch = 0;

    _vaddr = 0x0000;
    _tileAddr = 0x0000;
    _xaddr = 0x00;

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
    return (_bgEnable || _spriteEnable);
}

uint8_t Ppu::_spriteHeight() {
    return _spriteHeightMode == 0 ? 8 : 16;
}

uint8_t Ppu::_scrollX() {
    return ((_vaddr & 0x1f) << 3) | _xaddr;
}

uint8_t Ppu::_scrollY() {
    return (((_vaddr >> 5) & 0x1f) << 3) | ((_vaddr >> 12) & 7);
}

void Ppu::_scrollXIncrement() {
    if(_rasterEnable() == false) return;
    _vaddr = (_vaddr & 0x7fe0) | ((_vaddr + 0x0001) & 0x001f);
    if((_vaddr & 0x001f) == 0x0000) {
        _vaddr ^= 0x0400;
    }
}

void Ppu::_scrollYIncrement() {
    if(_rasterEnable() == false) return;
    _vaddr = (_vaddr & 0x0fff) | ((_vaddr + 0x1000) & 0x7000);
    if((_vaddr & 0x7000) == 0x0000) {
        _vaddr = (_vaddr & 0x7c1f) | ((_vaddr + 0x0020) & 0x03e0);
        if((_vaddr & 0x03e0) == 0x03c0) {  //0x03c0 == 30 << 5; 30 * 8 = 240
            _vaddr &= 0x7c1f;
            _vaddr ^= 0x0800;
        }
    }
}

void Ppu::_scanlineIncrement() {
    if(++_scanline == 262) {
        _scanline = 0;
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
            value |= _mbr & 0x1f;
            _vblank = 0;
            _system->cpu->Nmi(false);
            //status.address_latch = 0;
            break;
        case 4:  //OAMDATA
            value = _oam[_oamAddr];
            if((_oamAddr & 3) == 3) value &= 0xe3;
            break;
        case 7:  //PPUDATA
            if(_rasterEnable() && (_scanline <= 240 || _scanline == 261)) return 0x00;

            addr = _vaddr & 0x3fff;
            if(addr <= 0x1fff) {
                value = _data;
                _data = _system->cart->ChrRead(addr);
            } else if(addr <= 0x3eff) {
                value = _data;
                _data = _system->cart->ChrRead(addr);
            } else if(addr <= 0x3fff) {
                value = CgramRead(addr);
                _data = _system->cart->ChrRead(addr);
            }
            _vaddr += _incrementMode;
            break;
    }

    return value;
}

void Ppu::Write(uint16_t addr, uint8_t data){
    _mbr = data;

    switch(addr & 7) {
        case 0: //PPUCTRL
            _nmiEnable = data & 0x80;
            _system->cpu->Nmi(_nmiEnable && _vblank);
            _masterSlave = data & 0x40;
            _spriteHeightMode = data & 0x20;
            _bgTileSelect = (data & 0x10) ? 0x1000 : 0x0000;
            _spriteTileSelect = (data & 0x08) ? 0x1000 : 0x0000;
            _incrementMode = (data & 0x04) ? 32 : 1;
            _tileAddr = (_tileAddr & 0x73ff) | ((data & 0x03) << 10);
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
                _xaddr = data & 0x07;
                _tileAddr = (_tileAddr & 0x7fe0) | (data >> 3);
            } else {
                _tileAddr = (_tileAddr & 0x0c1f) | ((data & 0x07) << 12) | ((data >> 3) << 5);
            }
            _latch ^= 1;
            return;
        case 6: //PPUADDR
            if(_latch == 0) {
                _tileAddr = (_tileAddr & 0x00ff) | ((data & 0x3f) << 8);
            } else {
                _tileAddr = (_tileAddr & 0x7f00) | data;
                _vaddr = _tileAddr;
            }
            _latch ^= 1;
            return;
        case 7: //PPUDATA
            if(_rasterEnable() && (_scanline <= 240 || _scanline == 261)) return;

            addr = _vaddr & 0x3fff;
            if(addr <= 0x1fff) {
                _system->cart->ChrWrite(addr, data);
            } else if(addr <= 0x3eff) {
                _system->cart->ChrWrite(addr, data);
            } else if(addr <= 0x3fff) {
                CgramWrite(addr, data);
            }
            _vaddr += _incrementMode;
            return;
    }
}

uint8_t Ppu::ChrLoad(uint16_t addr){
    if(_rasterEnable() == false) return 0x00;
    return _system->cart->ChrRead(addr);
}

void Ppu::_frameEdge() {
    _field ^= 1;
    render = true;
}

void Ppu::_scanlineEdge() {
    if(_scanline == 241) {
        _vblank = 1;
        if(_nmiEnable) _system->cpu->Nmi(1);
    }
    if(_scanline == 261) {
        _vblank = 0;
        _system->cpu->Nmi(0);
        _spriteZeroHit = 0;
    }

    _dot = 0;

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

void Ppu::RasterPixel(unsigned x) {
    uint16_t *pixel = _screenbuffer + (_scanline * 256);

    unsigned mask = 0x8000 >> (_xaddr + x);
    unsigned palette = 0, object_palette = 0;
    bool object_priority = 0;
    palette |= (raster.tileLo & mask) ? 1 : 0;
    palette |= (raster.tileHi & mask) ? 2 : 0;
    if(palette) {
        unsigned attr = raster.attribute;
        if(mask >= 256) attr >>= 2;
        palette |= (attr & 3) << 2;
    }

    if(_bgEnable == false) palette = 0;
    if(_bgLColEnable == false && _dot < 8) palette = 0;

    for(signed sprite = 7; sprite >= 0; sprite--) {
        if(_spriteLColEnable == false && _dot < 8) continue;
        if(raster.oam[sprite].id == 64) continue;

        unsigned spritex = _dot - raster.oam[sprite].x;
        if(spritex >= 8) continue;

        if(raster.oam[sprite].attributes & 0x40) spritex ^= 7;
        unsigned mask = 0x80 >> spritex;
        unsigned sprite_palette = 0;
        sprite_palette |= (raster.oam[sprite].tileLo & mask) ? 1 : 0;
        sprite_palette |= (raster.oam[sprite].tileHi & mask) ? 2 : 0;
        if(sprite_palette == 0) continue;

        if(raster.oam[sprite].id == 0 && palette) _spriteZeroHit = 1;
        sprite_palette |= (raster.oam[sprite].attributes & 3) << 2;

        object_priority = raster.oam[sprite].attributes & 0x20;
        object_palette = 16 + sprite_palette;
    }

    if(object_palette) {
        if(palette == 0 || object_priority == 0) palette = object_palette;
    }

    if(_rasterEnable() == false) palette = 0;
    pixel[_dot] = (_bgrEmphasis << 6) | CgramRead(palette);
}

void Ppu::RasterSprite() {
    if(_spriteEnable == false) return;

    unsigned n = raster.iterator++;
    signed ly = (_scanline == 261 ? -1 : _scanline);
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
}

void Ppu::Cycle() {
    if (_scanline == 261) _visibleScanline();
    else if (_scanline >= 0 && _scanline < 240) _visibleScanline();
    else if (_scanline >= 240 && _scanline < 261) _verticalBlankingLine();
}

void Ppu::_preRenderScanline() {

}

void Ppu::_visibleScanline() {

    if (_dot < 256) _visibleDot();
    if (_dot >= 256 && _dot < 320) _fetchSpriteDataForNextScanline();
    if (_dot >= 320 && _dot < 336) _fetchTileDataForNextScanline();
    if (_dot >= 336 && _dot < 340) _fetchNameTable();
    if (_dot == 340) _addClocks();

    _dot++;
    if (_dot == 341) _scanlineIncrement();
}

void Ppu::_visibleDot() {
    unsigned tile = _dot / 32;
    unsigned pixel = (_dot - (tile * 32)) / 8;

    switch (pixel){
        case 0 :
            _nametableLatch = ChrLoad(0x2000 | (_vaddr & 0x0fff));
            _tileAddrLatch = _bgTileSelect + (_nametableLatch << 4) + (_scrollY() & 7);
            RasterPixel(pixel);
            _addClocks();
            break;
        case 1:
            RasterPixel(pixel);
            _addClocks();
            break;
        case 2:
            _attributeLatch = ChrLoad(0x23c0 | (_vaddr & 0x0fc0) | ((_scrollY() >> 5) << 3) | (_scrollX() >> 5));
            if(_scrollY() & 16) _attributeLatch >>= 4;
            if(_scrollX() & 16) _attributeLatch >>= 2;
            RasterPixel(pixel);
            _addClocks();
            break;
        case 3:
            _scrollXIncrement();
            if(tile == 31) _scrollYIncrement();
            RasterPixel(pixel);
            RasterSprite();
            _addClocks();
            break;
        case 4:
            _tileLoLatch = ChrLoad(_tileAddrLatch + 0);
            RasterPixel(pixel);
            _addClocks();
            break;
        case 5:
            RasterPixel(pixel);
            _addClocks();
            break;
        case 6:
            _tileHiLatch = ChrLoad(_tileAddrLatch + 8);
            RasterPixel(pixel);
            _addClocks();
            break;
        case 7:
            RasterPixel(7);
            RasterSprite();
            _addClocks();

            raster.nametable = (raster.nametable << 8) | _nametableLatch;
            raster.attribute = (raster.attribute << 2) | (_attributeLatch & 3);
            raster.tileLo = (raster.tileLo << 8) | _tileLoLatch;
            raster.tileHi = (raster.tileHi << 8) | _tileHiLatch;
            break;
    }
}

void Ppu::_fetchSpriteDataForNextScanline() {
    if (_dot == 256) {
        for(unsigned n = 0; n < 8; n++) raster.oam[n] = raster.soam[n];
    }

    unsigned sprite = (_dot - 256) / 8;
    unsigned spriteOffset = _dot - (sprite * 8) - 256;

    switch (spriteOffset){
        case 0:
            _nametableLatch = ChrLoad(0x2000 | (_vaddr & 0x0fff));
            _addClocks();
            break;
        case 1:
            if(_rasterEnable() && sprite == 0) _vaddr = (_vaddr & 0x7be0) | (_tileAddr & 0x041f);
            _addClocks();
            break;
        case 2:
            _attributeLatch = ChrLoad(0x23c0 | (_vaddr & 0x0fc0) | ((_scrollY() >> 5) << 3) | (_scrollX() >> 5));
            _tileAddrLatch = (_spriteHeight() == 8)
                                ? _spriteTileSelect + raster.oam[sprite].tile * 16
                                : ((raster.oam[sprite].tile & ~1) * 16) + ((raster.oam[sprite].tile & 1) * 0x1000);
            _addClocks();
            break;
        case 3:
            _addClocks();
            break;
        case 4: {
            unsigned spritey = (_scanline - raster.oam[sprite].y) & (_spriteHeight() - 1);
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
            if(_rasterEnable() && sprite == 6 && _scanline == 261) _vaddr = _tileAddr;
            break;
    }
}

void Ppu::_fetchTileDataForNextScanline() {
    unsigned tile = (_dot - 320) / 8;
    unsigned tileOffset = _dot - (tile * 8) - 320;

    switch(tileOffset) {
        case 0:
            _nametableLatch = ChrLoad(0x2000 | (_vaddr & 0x0fff));
            _tileAddrLatch = _bgTileSelect + (_nametableLatch << 4) + (_scrollY() & 7);
            _addClocks();
            break;
        case 1:
            _addClocks();
            break;
        case 2:
            _attributeLatch = ChrLoad(0x23c0 | (_vaddr & 0x0fc0) | ((_scrollY() >> 5) << 3) | (_scrollX() >> 5));
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
            break;
    }
}

void Ppu::_fetchNameTable() {
    switch(_dot){
        case 336:
            ChrLoad(0x2000 | (_vaddr & 0x0fff));
            _addClocks();
            break;
        case 337:
            _addClocks();
            break;
        case 338:
            ChrLoad(0x2000 | (_vaddr & 0x0fff));
            _addClocks();
            break;
        case 339:
            _addClocks();
            break;
    }
}

void Ppu::_verticalBlankingLine() {
    if (_dot < 340) _addClocks();
    else {
        if(_rasterEnable() == false || _field != 1 || _scanline != 240) _addClocks();
        return _scanlineIncrement();
    }
    _dot++;
}

uint16_t* Ppu::ScreenBuffer() {
    render = false;
    return _screenbuffer;
}