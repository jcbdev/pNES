//
// Created by James Barker on 08/01/2018.
//

#include "Ppu.h"

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

uint8_t Ppu::Read(uint16_t addr) {
    return 0;
}

void Ppu::Write(uint16_t addr, uint8_t data) {

}

uint8_t Read(uint16_t addr){
    uint8_t result = 0x00;

//    switch(addr & 7) {
//        case 2:  //PPUSTATUS
//            result |= _nmi << 7;
//            result |= _spriteZeroHit << 6;
//            result |= _sprite_overflow << 5;
//            result |= status.mdr & 0x1f;
//            _nmi = 0;
//            cpu.set_nmi_line(0);
//            status.address_latch = 0;
//            break;
//        case 4:  //OAMDATA
//            result = oam[status.oam_addr];
//            if((status.oam_addr & 3) == 3) result &= 0xe3;
//            break;
//        case 7:  //PPUDATA
//            if(raster_enable() && (status.ly <= 240 || status.ly == 261)) return 0x00;
//
//            addr = status.vaddr & 0x3fff;
//            if(addr <= 0x1fff) {
//                result = status.bus_data;
//                status.bus_data = cartridge.chr_read(addr);
//            } else if(addr <= 0x3eff) {
//                result = status.bus_data;
//                status.bus_data = cartridge.chr_read(addr);
//            } else if(addr <= 0x3fff) {
//                result = cgram_read(addr);
//                status.bus_data = cartridge.chr_read(addr);
//            }
//            status.vaddr += status.vram_increment;
//            break;
//    }

    return result;
}

void Write(uint16_t addr, uint8_t data){

}