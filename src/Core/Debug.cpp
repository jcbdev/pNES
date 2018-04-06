//
// Created by jimbo on 02/04/18.
//

#include <iomanip>
#include <algorithm>
#include "Debug.h"
#include "Cpu.h"
#include "Memory.h"
#include "Ppu.h"

#define hex2 std::setfill('0') << std::setw(2) << std::hex
#define hex4 std::setfill('0') << std::setw(4) << std::hex
#define hex8 std::setfill('0') << std::setw(8) << std::hex

#define op(byte, prefix, mode, length) \
    case byte: \
        disasm = Disassembly(); \
        output << hex4 << (int)pc; \
        setvalue(disasm.address); \
        length(); \
        setvalue(disasm.bytes); \
        output << #prefix; \
        mode(); \
        setvalue(disasm.assembly); \
        disassembly[pc] = disasm; \
    break

#define setvalue(val) \
    val = output.str(); \
    output = std::stringstream();


IDebug::IDebug(ISystem *system){
    this->_system = system;
}

Debug::Debug(ISystem *system) : IDebug::IDebug(system) {
    //breakpoints.push_back(0x814d);
    breakpoints.push_back(0x8150);
    step = true;
    pause = true;
}

std::string Debug::_decode(int pc, int* increment) {
    std::stringstream output;
    Disassembly disasm;

    auto abs = [&]() -> void { output << " $" << hex2 << (int)_system->mem->Read(pc + 2) << hex2 << (int) _system->mem->Read(pc + 1); };
    auto abx = [&]() -> void { output << " $" << hex2 << (int)_system->mem->Read(pc + 2) << hex2 << (int)_system->mem->Read(pc + 1) << ",x"; };
    auto aby = [&]() -> void { output << " $" << hex2 << (int)_system->mem->Read(pc + 2) << hex2 << (int)_system->mem->Read(pc + 1) << ",y"; };
    auto iab = [&]() -> void { output << " ($" << hex2 << (int)_system->mem->Read(pc + 2) << hex2 << (int)_system->mem->Read(pc + 1) << ")"; };
    auto imm = [&]() -> void { output << " #$" << hex2 << (int)_system->mem->Read(pc + 1); };
    auto imp = [&]() -> void { output << "       "; };
    auto izx = [&]() -> void { output << " ($" << hex2 << (int)_system->mem->Read(pc + 1) << ",x)"; };
    auto izy = [&]() -> void { output << " ($" << hex2 << (int)_system->mem->Read(pc + 1) << "),y"; };
    auto rel = [&]() -> void { output << " $" << hex4 << (int)(pc + 2 + (int8_t)_system->mem->Read(pc + 1)); };
    auto zpg = [&]() -> void { output << " $" << hex2 << (int)_system->mem->Read(pc + 1); };
    auto zpx = [&]() -> void { output << " $" << hex2 << (int)_system->mem->Read(pc + 1) << ",x"; };
    auto zpy = [&]() -> void { output << " $" << hex2 << (int)_system->mem->Read(pc + 1) << ",y"; };

    auto one   = [&]() -> void { *increment = 1; output << hex2 << (int)_system->mem->Read(pc); };
    auto two   = [&]() -> void { *increment = 2; output << hex2 << (int)_system->mem->Read(pc) << " " << hex2 << (int)_system->mem->Read(pc + 1); };
    auto three = [&]() -> void { *increment = 3; output << hex2 << (int)_system->mem->Read(pc) << " " << hex2 << (int)_system->mem->Read(pc + 1) << " " << hex2 << (int)_system->mem->Read(pc + 2); };

    uint8_t opcode = _system->mem->Read(pc);
    switch(opcode) {
        op(0x00, brk, imm, two);
        op(0x01, ora, izx, two);
        op(0x05, ora, zpg, two);
        op(0x06, asl, zpg, two);
        op(0x08, php, imp, one);
        op(0x09, ora, imm, two);
        op(0x0a, asl, imp, one);
        op(0x0d, ora, abs, three);
        op(0x0e, asl, abs, three);
        op(0x10, bpl, rel, two);
        op(0x11, ora, izy, two);
        op(0x15, ora, zpx, two);
        op(0x16, asl, zpx, two);
        op(0x18, clc, imp, one);
        op(0x19, ora, aby, three);
        op(0x1d, ora, abx, three);
        op(0x1e, asl, abx, three);
        op(0x20, jsr, abs, three);
        op(0x21, and, izx, two);
        op(0x24, bit, zpg, two);
        op(0x25, and, zpg, two);
        op(0x26, rol, zpg, two);
        op(0x28, plp, imp, one);
        op(0x29, and, imm, two);
        op(0x2a, rol, imp, one);
        op(0x2c, bit, abs, three);
        op(0x2d, and, abs, three);
        op(0x2e, rol, abs, three);
        op(0x30, bmi, rel, two);
        op(0x31, and, izy, two);
        op(0x35, and, zpx, two);
        op(0x36, rol, zpx, two);
        op(0x38, sec, imp, one);
        op(0x39, and, aby, three);
        op(0x3d, and, abx, three);
        op(0x3e, rol, abx, three);
        op(0x40, rti, imp, one);
        op(0x41, eor, izx, two);
        op(0x45, eor, zpg, two);
        op(0x46, lsr, zpg, two);
        op(0x48, pha, imp, one);
        op(0x49, eor, imm, two);
        op(0x4a, lsr, imp, one);
        op(0x4c, jmp, abs, three);
        op(0x4d, eor, abs, three);
        op(0x4e, lsr, abs, three);
        op(0x50, bvc, rel, two);
        op(0x51, eor, izy, two);
        op(0x55, eor, zpx, two);
        op(0x56, lsr, zpx, two);
        op(0x58, cli, imp, one);
        op(0x59, eor, aby, three);
        op(0x5a, phy, imp, one);
        op(0x5d, eor, abx, three);
        op(0x5e, lsr, abx, three);
        op(0x60, rts, imp, one);
        op(0x61, adc, izx, two);
        op(0x65, adc, zpg, two);
        op(0x66, ror, zpg, two);
        op(0x68, pla, imp, one);
        op(0x69, adc, imm, two);
        op(0x6a, ror, imp, one);
        op(0x6c, jmp, iab, three);
        op(0x6d, adc, abs, three);
        op(0x6e, ror, abs, three);
        op(0x70, bvs, rel, two);
        op(0x71, adc, izy, two);
        op(0x75, adc, zpx, two);
        op(0x76, ror, zpx, two);
        op(0x78, sei, imp, one);
        op(0x79, adc, aby, three);
        op(0x7a, ply, imp, one);
        op(0x7d, adc, abx, three);
        op(0x7e, ror, abx, three);
        op(0x81, sta, izx, two);
        op(0x84, sty, zpg, two);
        op(0x85, sta, zpg, two);
        op(0x86, stx, zpg, two);
        op(0x88, dey, imp, one);
        op(0x8a, txa, imp, one);
        op(0x8c, sty, abs, three);
        op(0x8d, sta, abs, three);
        op(0x8e, stx, abs, three);
        op(0x90, bcc, rel, two);
        op(0x91, sta, izy, two);
        op(0x94, sty, zpx, two);
        op(0x95, sta, zpx, two);
        op(0x96, stx, zpy, two);
        op(0x98, tya, imp, one);
        op(0x99, sta, aby, three);
        op(0x9a, txs, imp, one);
        op(0x9d, sta, abx, three);
        op(0xa0, ldy, imm, two);
        op(0xa1, lda, izx, two);
        op(0xa2, ldx, imm, two);
        op(0xa4, ldy, zpg, two);
        op(0xa5, lda, zpg, two);
        op(0xa6, ldx, zpg, two);
        op(0xa8, tay, imp, one);
        op(0xa9, lda, imm, two);
        op(0xaa, tax, imp, one);
        op(0xac, ldy, abs, three);
        op(0xad, lda, abs, three);
        op(0xae, ldx, abs, three);
        op(0xb0, bcs, rel, two);
        op(0xb1, lda, izy, two);
        op(0xb4, ldy, zpx, two);
        op(0xb5, lda, zpx, two);
        op(0xb6, ldx, zpy, two);
        op(0xb8, clv, imp, one);
        op(0xb9, lda, aby, three);
        op(0xba, tsx, imp, one);
        op(0xbc, ldy, abx, three);
        op(0xbd, lda, abx, three);
        op(0xbe, ldx, aby, three);
        op(0xc0, cpy, imm, two);
        op(0xc1, cmp, izx, two);
        op(0xc4, cpy, zpg, two);
        op(0xc5, cmp, zpg, two);
        op(0xc6, dec, zpg, two);
        op(0xc8, iny, imp, one);
        op(0xc9, cmp, imm, two);
        op(0xca, dex, imp, one);
        op(0xcc, cpy, abs, three);
        op(0xcd, cmp, abs, three);
        op(0xce, dec, abs, three);
        op(0xd0, bne, rel, two);
        op(0xd1, cmp, izy, two);
        op(0xd5, cmp, zpx, two);
        op(0xd6, dec, zpx, two);
        op(0xd8, cld, imp, one);
        op(0xd9, cmp, aby, three);
        op(0xda, phx, imp, one);
        op(0xdd, cmp, abx, three);
        op(0xde, dec, abx, three);
        op(0xe0, cpx, imm, two);
        op(0xe1, sbc, izx, two);
        op(0xe4, cpx, zpg, two);
        op(0xe5, sbc, zpg, two);
        op(0xe6, inc, zpg, two);
        op(0xe8, inx, imp, one);
        op(0xe9, sbc, imm, two);
        op(0xec, cpx, abs, three);
        op(0xed, sbc, abs, three);
        op(0xee, inc, abs, three);
        op(0xf0, beq, rel, two);
        op(0xf1, sbc, izy, two);
        op(0xf5, sbc, zpx, two);
        op(0xf6, inc, zpx, two);
        op(0xf8, sed, imp, one);
        op(0xf9, sbc, aby, three);
        op(0xfa, plx, imp, one);
        op(0xfd, sbc, abx, three);
        op(0xfe, inc, abx, three);

        default:
            disasm = Disassembly();
            output << hex4 << (int)pc;
            setvalue(disasm.address);
            one();
            setvalue(disasm.bytes);
            output << ".db $" << hex2 << (int)opcode;
            setvalue(disasm.assembly);
            disassembly[pc] = disasm;
            break;
    }

    return output.str();
}

void Debug::_setStatus() {
    std::stringstream output;

    output << "PC: " << hex4 << (int)_system->cpu->pc;
    setvalue(status.pc);
    output << "A: " << hex2 << (int)_system->cpu->a;
    setvalue(status.a);
    output << "X: " << hex2 << (int)_system->cpu->x;
    setvalue(status.x);
    output << "Y: " << hex2 << (int)_system->cpu->y;
    setvalue(status.y);
    output << "SP: " << hex2 << (int)_system->cpu->s;
    setvalue(status.sp);
    output << "P: " << (_system->cpu->p.n ? "N" : "n")
           << (_system->cpu->p.v ? "V" : "v")
           << (_system->cpu->p.d ? "D" : "d")
           << (_system->cpu->p.i ? "I" : "i")
           << (_system->cpu->p.z ? "Z" : "z")
           << (_system->cpu->p.c ? "C" : "c")
           << "(" << hex2 << (int)_system->cpu->p << ")";
    setvalue(status.p);

    output << "Clocks: " << hex8 << (int)(_system->totalClocks);
    setvalue(status.clock);
    output << "Scanline: " << hex2 << (int)(_system->ppu->scanline);
    setvalue(status.scanline);
    output << "Dot: " << hex2 << (int)_system->ppu->dot;
    setvalue(status.dot);
    output << "XOffset: " << hex2 << (int)_system->ppu->xaddr;
    setvalue(status.xaddr);
    output << "VAddr: " << hex4 << (int)_system->ppu->vaddr;
    setvalue(status.vaddr);
    output << "TAddr: " << hex4 << (int)_system->ppu->tileAddr;
    setvalue(status.taddr);
    output << "Buffer: " << hex2 << (int)_system->ppu->buffer;
    setvalue(status.buffer)
    output << "PPU0: " << hex2 << (int)_system->ppu->PPUSTATUS();
    setvalue(status.PPU0)
    output << "PPU1: " << hex2 << (int)_system->ppu->PPUMASK();
    setvalue(status.PPU1)
    output << "PPU2: " << hex2 << (int)_system->ppu->PPUCTRL();
    setvalue(status.PPU2)
    output << "PPU3: " << hex2 << (int)_system->ppu->OAMADDR();
    setvalue(status.PPU3)
    output << "V-Toggle: " << hex2 << (int)_system->ppu->frameToggle;
    setvalue(status.frame)
}

void Debug::Refresh() {
    if (!_disassembled) {
        _preDisassemble();
    }
    _setStatus();
    if (isBrk(_system->cpu->pc) && !step && !pause) {
        pause = true;
        cursorPosition = _system->cpu->pc;
    }
    if (!pause || (step && pause)) {
        cursorPosition = _system->cpu->pc;
    }
}

void Debug::GoNext() {
    auto iterator = disassembly.find(cursorPosition);
    iterator++;
    if (iterator == disassembly.end()) iterator = disassembly.begin();
    cursorPosition = iterator->first;
}

void Debug::GoPrev(){
    auto iterator = disassembly.find(cursorPosition);
    iterator--;
    if (iterator == disassembly.begin()) iterator = disassembly.end();
    cursorPosition = iterator->first;
}

void Debug::AddBrk() {
    if(isBrk(cursorPosition))
    {
        breakpoints.remove(cursorPosition);
    }
    else breakpoints.push_back(cursorPosition);
}

bool Debug::isBrk(int line){
    for(uint16_t v : breakpoints){
        if (line == v)
            return true;
    }
    return false;
}

//std::string Cpu::Disassemble() {
//    std::stringstream output;
//    auto iterator = _disassembly.find(pc);
//    int back = 0;
//    //Get start item
//    while (iterator != _disassembly.begin() && back < 10){
//        back++;
//        iterator--;
//    }
//    //Now output commands
//    int forward = 0;
//    while (iterator != _disassembly.end() && forward < 20){
//        output << (pc == iterator->first ? "*" : " ") << iterator->second << std::endl;
//        forward++;
//        iterator++;
//    }
//    return output.str();
//}

void Debug::_preDisassemble() {
    int curPc = 0x8000;
    int opcodeSize = 0;
    while (curPc<=0xFFFF) {
        _decode(curPc, &opcodeSize);
        curPc += opcodeSize;
    }
    _disassembled = true;
}

#undef setvalue
#undef op
#undef hex8
#undef hex4
#undef hex2