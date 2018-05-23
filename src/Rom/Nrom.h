//
// Created by James Barker on 07/10/2017.
//

#ifndef LITTLEPNES_NROM_H
#define LITTLEPNES_NROM_H

#include "Cart.h"

class Nrom : public Cart {
public:
    uint8_t Read(uint16_t addr) override;
    void Write(uint16_t addr, uint8_t data) override;

    Nrom(ISystem *system);
    ~Nrom();
};


#endif //LITTLEPNES_NROM_H
