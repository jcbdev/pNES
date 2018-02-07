//
// Created by James Barker on 03/10/2017.
//

#include "Cart.h"
#include "../Helpers/Logger.h"
#include <cstring>
#include <algorithm>

void Cart::LoadRom(std::string romFile) {
    _system->logger->Log("Loading Rom " + romFile);

    std::ifstream t(romFile);

    if (t.fail()){
        _system->logger->Log("Cannot load rom");
        exit(0);
    }

    t.seekg(0, std::ios::end);
    romSize = t.tellg();
    _system->logger->Log("Loading rom of size: " + std::to_string(romSize) + "...");
    //romData = new char[romSize];
    romData = (char*)malloc(romSize);
    t.seekg(0);
    t.read(&romData[0], romSize);
    _system->logger->Log("Loaded");

    ExtractHeader();
    DetectType();
    LogHeader();
    SetBanks();
}

void Cart::ExtractHeader(){
    _system->logger->Log("Extracting Header");
    memcpy(&Header, &romData[0], sizeof(RomHeader));

    std::string marker;
    marker.assign(Header.Marker, 4);
    _system->logger->Log("-- Marker: " + marker);
}

void Cart::LogHeader(){
    _system->logger->Log("---- PRG ROM Size: " + std::to_string(Header.PrgRomSize * 0x4000));
    _system->logger->Log("---- CHR ROM Size: " + std::to_string(Header.ChrRomSize * 0x2000));
    _system->logger->Log("---- Mapper: " + std::to_string(Header.LowerMapper() + (Header.UpperMapper() << 4)));
    _system->logger->Log("---- Mirroring: " + std::to_string(Header.Mirroring()));
    _system->logger->Log("---- Battery Backed PRG RAM: " + std::to_string(Header.BatteryBackedPRGRam()));
    _system->logger->Log("---- Trainer: " + std::to_string(Header.Trainer()));
    _system->logger->Log("---- Four Screen Mode: " + std::to_string(Header.FourScreenMode()));
    _system->logger->Log("---- Trainer: " + std::to_string(Header.Trainer()));
    _system->logger->Log("---- Vs. Unisystem: " + std::to_string(Header.VsUnisystem()));
    _system->logger->Log("---- Playchoice 10: " + std::to_string(Header.PlayChoice10()));
    if (Type == INes)
    {
        _system->logger->Log("---- Tv System: " + std::to_string(Header.TvSystem()));
        _system->logger->Log("---- PRG RAM Present: " + std::to_string(Header.PrgRamPresent()));
        _system->logger->Log("---- Bus Conflicts: " + std::to_string(Header.BusConflicts()));
    }
    if (Type == INesV2)
    {
        _system->logger->Log("---- V2 Mapper: " + std::to_string((Header.LowerMapper() + (Header.UpperMapper() << 4) + (Header.UpperMapper() << 8))));
        _system->logger->Log("---- Submapper: " + std::to_string(Header.SubmapperNumber()));
        _system->logger->Log("---- V2 PRG ROM Size: " + std::to_string((Header.PrgRomSize + (Header.PrgRomHigher() << 8)) * 0x4000));
        _system->logger->Log("---- V2 CHR ROM Size: " + std::to_string((Header.ChrRomSize + (Header.ChrRomHigher() << 8)) * 0x2000));
        _system->logger->Log("---- PRG RAM Battery Backed: " + std::to_string(Header.EEPRomQuantity()));
        _system->logger->Log("---- PRG RAM Not Battery Backed: " + std::to_string(Header.PrgRamQuantity()));
        _system->logger->Log("---- CHR RAM Battery Backed: " + std::to_string(Header.ChrRamBBQuantity()));
        _system->logger->Log("---- CHR RAM Not Battery Backed: " + std::to_string(Header.ChrRamQuantity()));
        _system->logger->Log("---- V2 Tv System: " + std::to_string(Header.NtscOrPal()));
        _system->logger->Log("---- PPU: " + std::to_string(Header.PPU()));
        _system->logger->Log("---- VsMode: " + std::to_string(Header.VsMode()));
        _system->logger->Log("---- Extra Roms: " + std::to_string(Header.ExtraRoms()));
    }
}

void Cart::DetectType(){
    if ((Header.Flags7 & 0x0C) == 0x08
            && ((Header.PrgRomSize + (Header.RomSizeUpperBits & 0x0F)) * 0x4000)
            + ((Header.ChrRomSize + ((Header.RomSizeUpperBits >> 4) & 0xF0)) * 0x2000) + 16 <= romSize) {
        Type = INesV2;
        _system->logger->Log("-- Type: INESV2");
    }
    else if ((Header.Flags7 & 0x0C) == 0x08
            && (std::all_of(std::begin(Header.INes_Unused), std::end(Header.INes_Unused), [](int i){return i == 0;}))) {
        Type = INes;
        _system->logger->Log("-- Type: INES");
    }
    else {
        Type = Archaic;
        _system->logger->Log("-- Type: INES Archaic");
    }

}

void Cart::SetBanks() {
    auto prgsize=0;
    auto chrsize=0;
    if (Type == Archaic || Type == INes){
        prgsize = Header.PrgRomSize;
        chrsize = Header.ChrRomSize;
    }
    else {
        prgsize = (Header.PrgRomSize + (Header.PrgRomHigher() << 8));
        chrsize = (Header.ChrRomSize + (Header.ChrRomHigher() << 8));
    }
//    prgData = new uint8_t*[prgsize];
//    for (int i=0; i<prgsize; i++) {
//        prgData[i] = (uint8_t*) &romData[(uint8_t)(16 + (i * 0x4000))];
//    }
//
//    chrData = new uint8_t*[chrsize];
//    for (int i=0; i<chrsize; i++) {
//        chrData[i] = (uint8_t *)&romData[(uint8_t)(16 + ((prgsize - 1) * 0x4000) + (i * 0x2000))];
//    }

    LowerPrgBank = 0;
    UpperPrgBank = Header.PrgRomSize-1;
}

Cart::Cart(ISystem *system) {
    this->_system = system;
}

Cart::~Cart(){
    free(romData);
//    free(prgData);
//    free(chrData);
}

uint8_t Cart::PrgRead(uint16_t addr) {
    return 0;
}

void Cart::PrgWrite(uint16_t addr, uint8_t data) {}

uint8_t Cart::ChrRead(uint16_t addr) {
    return 0;
}

void Cart::ChrWrite(uint16_t addr, uint8_t data) {}