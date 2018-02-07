//
// Created by James Barker on 03/10/2017.
//

#include "Cart.h"

void Cart::LoadRom(std::string romFile) {
    logger->Log("Loading Rom " + romFile);

    std::ifstream t(romFile);

    if (t.fail()){
        logger->Log("Cannot load rom");
        exit(0);
    }

    t.seekg(0, std::ios::end);
    romSize = t.tellg();
    logger->Log("Loading rom of size: " + std::to_string(romSize) + "...");
    //romData = new char[romSize];
    romData = (char*)malloc(romSize);
    t.seekg(0);
    t.read(&romData[0], romSize);
    logger->Log("Loaded");

    ExtractHeader();
    DetectType();
    LogHeader();
    SetBanks();
}

void Cart::ExtractHeader(){
    logger->Log("Extracting Header");
    memcpy(&Header, &romData[0], sizeof(RomHeader));

    std::string marker;
    marker.assign(Header.Marker, 4);
    logger->Log("-- Marker: " + marker);
}

void Cart::LogHeader(){
    logger->Log("---- PRG ROM Size: " + std::to_string(Header.PrgRomSize * 0x4000));
    logger->Log("---- CHR ROM Size: " + std::to_string(Header.ChrRomSize * 0x2000));
    logger->Log("---- Mapper: " + std::to_string(Header.LowerMapper() + (Header.UpperMapper() << 4)));
    logger->Log("---- Mirroring: " + std::to_string(Header.Mirroring()));
    logger->Log("---- Battery Backed PRG RAM: " + std::to_string(Header.BatteryBackedPRGRam()));
    logger->Log("---- Trainer: " + std::to_string(Header.Trainer()));
    logger->Log("---- Four Screen Mode: " + std::to_string(Header.FourScreenMode()));
    logger->Log("---- Trainer: " + std::to_string(Header.Trainer()));
    logger->Log("---- Vs. Unisystem: " + std::to_string(Header.VsUnisystem()));
    logger->Log("---- Playchoice 10: " + std::to_string(Header.PlayChoice10()));
    if (Type == INes)
    {
        logger->Log("---- Tv System: " + std::to_string(Header.TvSystem()));
        logger->Log("---- PRG RAM Present: " + std::to_string(Header.PrgRamPresent()));
        logger->Log("---- Bus Conflicts: " + std::to_string(Header.BusConflicts()));
    }
    if (Type == INesV2)
    {
        logger->Log("---- V2 Mapper: " + std::to_string((Header.LowerMapper() + (Header.UpperMapper() << 4) + (Header.UpperMapper() << 8))));
        logger->Log("---- Submapper: " + std::to_string(Header.SubmapperNumber()));
        logger->Log("---- V2 PRG ROM Size: " + std::to_string((Header.PrgRomSize + (Header.PrgRomHigher() << 8)) * 0x4000));
        logger->Log("---- V2 CHR ROM Size: " + std::to_string((Header.ChrRomSize + (Header.ChrRomHigher() << 8)) * 0x2000));
        logger->Log("---- PRG RAM Battery Backed: " + std::to_string(Header.EEPRomQuantity()));
        logger->Log("---- PRG RAM Not Battery Backed: " + std::to_string(Header.PrgRamQuantity()));
        logger->Log("---- CHR RAM Battery Backed: " + std::to_string(Header.ChrRamBBQuantity()));
        logger->Log("---- CHR RAM Not Battery Backed: " + std::to_string(Header.ChrRamQuantity()));
        logger->Log("---- V2 Tv System: " + std::to_string(Header.NtscOrPal()));
        logger->Log("---- PPU: " + std::to_string(Header.PPU()));
        logger->Log("---- VsMode: " + std::to_string(Header.VsMode()));
        logger->Log("---- Extra Roms: " + std::to_string(Header.ExtraRoms()));
    }
}

void Cart::DetectType(){
    if ((Header.Flags7 & 0x0C) == 0x08
            && ((Header.PrgRomSize + (Header.RomSizeUpperBits & 0x0F)) * 0x4000)
            + ((Header.ChrRomSize + ((Header.RomSizeUpperBits >> 4) & 0xF0)) * 0x2000) + 16 <= romSize) {
        Type = INesV2;
        logger->Log("-- Type: INESV2");
    }
    else if ((Header.Flags7 & 0x0C) == 0x08
            && (std::all_of(std::begin(Header.INes_Unused), std::end(Header.INes_Unused), [](int i){return i == 0;}))) {
        Type = INes;
        logger->Log("-- Type: INES");
    }
    else {
        Type = Archaic;
        logger->Log("-- Type: INES Archaic");
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

Cart::Cart(ILogger *logger) {
    this->logger = logger;
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

uint8_t Cart::ChrRead(IPpu *ppu, uint16_t addr) {
    return 0;
}

void Cart::ChrWrite(IPpu *ppu, uint16_t addr, uint8_t data) {}