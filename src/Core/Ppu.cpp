//
// Created by James Barker on 08/01/2018.
//

#include "Ppu.h"
#include "Cpu.h"
#include "../Rom/Cart.h"

IPpu::IPpu(ISystem* system){
    _system = system;
}

Ppu::Ppu(ISystem* system) : IPpu::IPpu(system) {

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

void Ppu::Write(uint16_t addr, uint8_t data) {

}

bool Ppu::_rasterEnable() {
    return (_bgEnable || _spriteEnable);
}

uint8_t Ppu::Read(uint16_t addr){
    uint8_t value = 0x00;

    switch(addr & 7) {
        case 2:  //PPUSTATUS
            value |= _nmiEnable << 7;
            value |= _spriteZeroHit << 6;
            value |= _spriteOverflow << 5;
            value |= _mbr & 0x1f;
            _nmiEnable = 0;
            _system->cpu->Nmi(false);
            //status.address_latch = 0;
            break;
        case 4:  //OAMDATA
            value = _oam[_oamAddr];
            if((_oamAddr & 3) == 3) value &= 0xe3;
            break;
        case 7:  //PPUDATA
            if(_rasterEnable() && (_y <= 240 || _y == 261)) return 0x00;

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

void Write(uint16_t addr, uint8_t data){

}