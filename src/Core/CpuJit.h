//
// Created by jimbo on 26/07/19.
//

#ifndef PNES_CPUJIT_H
#define PNES_CPUJIT_H

#include <vector>
#include "Cpu.h"

struct simplenature {
    bool addr, x, y, s, a;
    bool n, v, d, i, z, c;

    uint16_t index () {
        return (addr << 12) | (x << 11) | (y << 10) | (s << 9) | (a << 8) |
            (n << 7) | (v << 6) | (d << 3) | (i << 2) | (z << 1) | (c << 0);
    }
};


class CpuJit : public ICpu {
public:
    explicit CpuJit(ISystem* system);

    void Reset() override;
    void Cycle() override;
    bool Interrupt() override;
    void Apu(bool line) override;
    void Irq(bool line) override;
    void Nmi(bool line) override;

private:

};


#endif //PNES_CPUJIT_H
