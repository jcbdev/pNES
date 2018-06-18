//
// Created by James Barker on 07/10/2017.
//

#ifndef LITTLEPNES_NROM_H
#define LITTLEPNES_NROM_H

#include "Cart.h"
#include <iomanip>
#include <string>
#include <sstream>

class Nrom : public Cart {
public:
    uint8_t Read(uint16_t addr) override;
    void Write(uint16_t addr, uint8_t data) override;

    Nrom(ISystem *system);
    ~Nrom();

private:
    template<class T> std::string _tohex(T value){
        std::stringstream stream;
        stream << "0x"
               << std::setfill ('0') << std::setw(sizeof(T)*2)
               << std::hex << (long)value << std::dec;
        return stream.str();
    }
};


#endif //LITTLEPNES_NROM_H
