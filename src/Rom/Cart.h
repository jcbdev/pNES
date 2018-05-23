//
// Created by James Barker on 03/10/2017.
//

#ifndef LITTLEPNES_CART_H
#define LITTLEPNES_CART_H

#include <string>
#include <fstream>
#include "../Core/System.h"
#include "RomHeader.h"
#include "RomTypes.h"

class Cart {
protected:
    uint32_t RomSize;
//    uint8_t** prgData;
//    uint8_t** chrData;
    uint8_t LowerPrgBank;
    uint8_t UpperPrgBank;
    uint8_t ChrBank;
    //char *romData;
    uint8_t **Prg;
    uint8_t **Chr;
    uint8_t* Sram;
    ISystem* _system;
    uint8_t ChrSize;
    uint8_t PrgSize;
    uint8_t MirrorMode;

private:
    void ExtractHeader(char *buffer);
    void LogHeader();
    void DetectType();
    void SetBanks(char *buffer);
public:
    RomHeader Header;
    RomType Type;

    void LoadRom(std::string romFile);
    Cart(ISystem *system);
    ~Cart();

    virtual uint8_t Read(uint16_t addr);
    virtual void Write(uint16_t addr, uint8_t data);
};


#endif //LITTLEPNES_CART_H
