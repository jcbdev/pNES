//
// Created by James Barker on 21/11/2016.
//

#ifndef LITTLEPNES_CORE_H
#define LITTLEPNES_CORE_H

#include <cstdint>

struct Address16 {
    union {
        uint16_t w;
        struct { uint8_t l, h; };
    };
};

struct CpuFlags {
    bool n, v, d, i, z, c;

    inline operator uint8_t() {
        return (n << 7) | (v << 6) | (d << 3) | (i << 2) | (z << 1) | (c << 0);
    }

    inline CpuFlags& operator=(uint8_t data) {
        n = data & 0x80;
        v = data & 0x40;
        d = data & 0x08;
        i = data & 0x04;
        z = data & 0x02;
        c = data & 0x01;
        return *this;
    }
};


#endif //LITTLEPNES_CORE_H
