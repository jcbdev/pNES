//
// Created by James Barker on 07/10/2017.
//

#ifndef LITTLEPNES_NROM_H
#define LITTLEPNES_NROM_H

#include "Cart.h"

class Nrom : public Cart {
public:
    uint8_t PrgRead(uint16_t addr);
    void PrgWrite(uint16_t addr, uint8_t data);
    uint8_t ChrRead(uint16_t addr);
    void ChrWrite(uint16_t addr, uint8_t data);

    Nrom(ILogger *logger);
    ~Nrom();
};


#endif //LITTLEPNES_NROM_H
