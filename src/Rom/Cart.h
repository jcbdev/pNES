//
// Created by James Barker on 03/10/2017.
//

#ifndef LITTLEPNES_CART_H
#define LITTLEPNES_CART_H

#include <string>
#include <fstream>
#include "../Helpers/Logger.h"
#include "RomHeader.h"
#include "RomTypes.h"
#include "../Core/Ppu.h"

class Cart {
protected:
    ILogger *logger;
    uint32_t romSize;
//    uint8_t** prgData;
//    uint8_t** chrData;
    uint8_t LowerPrgBank;
    uint8_t UpperPrgBank;
    uint8_t ChrBank;
    char *romData;

private:
    void ExtractHeader();
    void LogHeader();
    void DetectType();
    void SetBanks();
public:
    RomHeader Header;
    RomType Type;

    void LoadRom(std::string romFile);
    Cart(ILogger *logger);
    ~Cart();

    virtual uint8_t PrgRead(uint16_t addr);
    virtual void PrgWrite(uint16_t addr, uint8_t data);
    virtual uint8_t ChrRead(IPpu *ppu, uint16_t addr);
    virtual void ChrWrite(IPpu *ppu, uint16_t addr, uint8_t data);
};


#endif //LITTLEPNES_CART_H
