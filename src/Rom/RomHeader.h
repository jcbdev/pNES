//
// Created by James Barker on 03/10/2017.
//

#ifndef LITTLEPNES_ROMHEADER_H
#define LITTLEPNES_ROMHEADER_H

#include <cstdint>
#include <cmath>

struct RomHeader {
    char Marker[4];
    uint8_t PrgRomSize;
    uint8_t ChrRomSize;
    uint8_t Flags6;
    uint8_t Flags7;

    union {
        struct {
            char Archaic_Unused[8];
        };
        struct {
            uint8_t INes_PrgRamSize;
            uint8_t Flags9;
            uint8_t Flags10;
            char INes_Unused[5];
        };
        struct {
            uint8_t MapperVariant;
            uint8_t RomSizeUpperBits;
            uint8_t V2_PrgRamSize;
            uint8_t ChrRamSize;
            uint8_t V2_TvSystem;
            uint8_t VsHardware;
            uint8_t MiscRoms;
            uint8_t INesV2_Unused;
        };
    };

    bool Mirroring(){
        return (Flags6 & 0x0000000F) == 1;
    }

    bool BatteryBackedPRGRam() {
        return (Flags6 & 0x000000F0) == 1;
    }

    bool Trainer() {
        return (Flags6 & 0x00000F00) == 1;
    }

    bool FourScreenMode() {
        return (Flags6 & 0x0000F000) == 1;
    }

    uint8_t LowerMapper() {
        return (uint8_t) ((Flags6 & 0xFFFF0000) >> 4);
    }

    bool VsUnisystem(){
        return (Flags7 & 0x0000000F) == 1;
    }

    bool PlayChoice10() {
        return (Flags7 & 0x000000F0) == 1;
    }

    uint8_t UpperMapper() {
        return (uint8_t) ((Flags7 & 0xFFFF0000) >> 4);
    }

    uint8_t TvSystem(){
        return (uint8_t) (Flags10 & 0x000000FF);
    };

    bool PrgRamPresent(){
        return (Flags10 & 0x000F0000) == 1;
    };

    bool BusConflicts() {
        return (Flags10 & 0x00F00000) == 1;
    }

    uint8_t SubmapperNumber(){
        return (uint8_t) ((MapperVariant & 0xFFFF0000) >> 4);
    }

    uint8_t MapperHigh(){
        return (uint8_t) ((MapperVariant & 0x0000FFFF));
    }

    uint8_t PrgRomHigher(){
        return (uint8_t) ((RomSizeUpperBits & 0xFFFF0000) >> 4);
    }

    uint8_t ChrRomHigher(){
        return (uint8_t) ((RomSizeUpperBits & 0x0000FFFF));
    }

    uint8_t PrgRamQuantity(){
        uint8_t ramsize = (uint8_t) ((V2_PrgRamSize & 0xFFFF0000) >> 4);
        if (ramsize == 0) return 0;
        return (uint8_t) (128 * std::pow(2, ramsize));
    }

    uint8_t EEPRomQuantity(){
        uint8_t ramsize = (uint8_t) (V2_PrgRamSize & 0x0000FFFF);
        if (ramsize == 0) return 0;
        return (uint8_t) (128 * std::pow(2, ramsize));
    }

    uint8_t ChrRamBBQuantity(){
        uint8_t ramsize = (uint8_t) ((ChrRamSize & 0xFFFF0000) >> 4);
        if (ramsize == 0) return 0;
        return (uint8_t) (128 * std::pow(2, ramsize));
    }

    uint8_t ChrRamQuantity(){
        uint8_t ramsize = (uint8_t) ((ChrRamSize & 0x0000FFFF));
        if (ramsize == 0) return 0;
        return (uint8_t) (128 * std::pow(2, ramsize));
    }

    uint8_t NtscOrPal(){
        return (uint8_t) (V2_TvSystem & 0x0000000F);
    }

    uint8_t PPU(){
        return (uint8_t) ((VsHardware & 0xFFFF0000));
    }

    uint8_t VsMode(){
        return (uint8_t) ((VsHardware & 0x0000FFFF) >> 4);
    }

    uint8_t ExtraRoms(){
        return (uint8_t) ((MiscRoms & 0x000000FF));
    }
};

#endif //LITTLEPNES_ROMHEADER_H
