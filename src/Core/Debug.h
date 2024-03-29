//
// Created by jimbo on 02/04/18.
//

#ifndef PNES_DISASSEMBLY_H
#define PNES_DISASSEMBLY_H

#include <cstdint>
#include <map>
#include <sstream>
#include <list>
#include <vector>
#include "System.h"

template class std::vector<uint16_t>;

struct Disassembly {
    std::string address;
    std::string bytes;
    std::string assembly;
};

struct Status {
    std::string pc;
    std::string a;
    std::string x;
    std::string y;
    std::string sp;
    std::string p;
    std::string clock;

    std::string PPU0;
    std::string PPU1;
    std::string PPU2;
    std::string PPU3;
    std::string xaddr;
    std::string vaddr;
    std::string taddr;
    std::string buffer;
    std::string frame;
    std::string scanline;
    std::string dot;
};

class IDebug {
public:
    explicit IDebug(ISystem* system);

    Status status;
    std::map<uint16_t, Disassembly> disassembly;
    std::vector<uint16_t> trace;
    virtual std::string Decode(int pc, int* increment, bool dynamic) = 0;
    virtual void Refresh() = 0;
    virtual void GoNext() = 0;
    virtual void GoPrev() = 0;
    virtual void AddBrk() = 0;
    virtual bool isBrk(int line) = 0;
    virtual void Break() = 0;
    bool pause;
    bool step;
    bool enabled;
    uint16_t cursorPosition = 0;
    std::list<int16_t> breakpoints;

protected:
    ISystem *_system;
};

class Debug : public IDebug {
public:
    explicit Debug(ISystem* system);

    std::string Decode(int pc, int* increment, bool dynamic) override;
    void Refresh() override;
    void GoNext() override;
    void GoPrev() override;
    void AddBrk() override;
    bool isBrk(int line) override;
    void Break() override;
private:

    void _preDisassemble();
    void _setStatus();
    bool _disassembled = false;
};


#endif //PNES_DISASSEMBLY_H
