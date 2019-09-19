//
// Created by jimbo on 26/07/19.
//

#ifndef PNES_CPUJIT_H
#define PNES_CPUJIT_H

#include <vector>
#include "Cpu.h"
#include "Memory.h"
#include "System.h"
#include "../Helpers/Logger.h"


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
    std::string DecodeInstruction(int pc, int* increment, bool dynamic);

protected:
    ISystem *_system;
};


#endif //PNES_CPUJIT_H
