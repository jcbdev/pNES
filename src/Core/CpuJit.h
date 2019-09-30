//
// Created by jimbo on 26/07/19.
//

#ifndef PNES_CPUJIT_H
#define PNES_CPUJIT_H

#include <vector>
#include <map>
#include <exception>
#include "Cpu.h"
#include "Memory.h"
#include "System.h"
#include "../Helpers/Logger.h"
#include "../Modules/keystone/include/keystone/keystone.h"


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
    std::vector<std::string> currentBlock;
    void *block;
    std::string DecodeInstruction(int pc, int* increment, bool dynamic);
    std::string RecodeInstruction(int pc, int* increment);
    void Emit();
    void Exec();

protected:
    ISystem *_system;
    ICpu *_interpreter;
};


#endif //PNES_CPUJIT_H
