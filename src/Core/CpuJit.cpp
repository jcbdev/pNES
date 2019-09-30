//
// Created by jimbo on 26/07/19.
//

#include <memory.h>
#include <sys/mman.h>
#include "CpuJit.h"

struct Disassembly {
    std::string address;
    std::string bytes;
    std::string assembly;
};

struct state {
    uint8_t ram[0x800];
    uint16_t pc;
    uint8_t x;
    uint8_t y;
    uint8_t a;
    uint8_t s;
    bool n, v, d, i, z, c;
    uint8_t val;
    uint16_t addr;
    int32_t clocks;
    int64_t cycle;
    bool irq;
    bool apu;
    bool nmiPending;
    bool interruptPending;

} cpuState;

#define hex2 std::setfill('0') << std::setw(2) << std::hex
#define hex4 std::setfill('0') << std::setw(4) << std::hex
#define hex8 std::setfill('0') << std::setw(8) << std::hex

#define op(byte, prefix, mode, length) \
    case byte: \
        disasm = Disassembly(); \
        if (dynamic) output << "!"; \
        output << hex4 << (int)pc; \
        setvalue(disasm.address); \
        length(); \
        setvalue(disasm.bytes); \
        output << #prefix; \
        mode(); \
        setvalue(disasm.assembly); \
    break

#define setvalue(val) \
    val = output.str(); \
    output = std::stringstream();

std::string CpuJit::DecodeInstruction(int pc, int* increment, bool dynamic) {
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
        op(0x00, brk, imm, one);
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
            //disassembly[pc] = disasm;
            break;
    }

    _system->logger->Log(disasm.address + "  " + disasm.bytes + "  " + disasm.assembly);
    return disasm.address + "  " + disasm.bytes + "  " + disasm.assembly;
}

#define echo(str) \
    currentBlock.push_back(str); \
    _system->logger->Log(str); \

#define reop(byte, prefix, mode, length) \
    case byte: \
        prefix(); \
        mode(); \
    break
// -----------------------------------------------------------------------------------------
#define preamble() \
    echo("%define PC word ptr [rdi + 2048]") \
    echo("%define XREG word ptr [rdi + 2050]") \
    echo("%define YREG word ptr [rdi + 2051]") \

// -----------------------------------------------------------------------------------------
#define read_addr() \
    read_ram() \

#define read_ram() \
    echo("movzx eax, word ptr [rdi + 2062]") \
    echo("and eax, 2047") \
    echo("mov al, byte ptr [rdi + rax]") \
    echo("mov byte ptr [rdi + 2060], al") \

#define read_pc() \
    echo("movzx eax, word ptr [rdi + 2048]") \
    echo("mov word ptr [rdi + 2062], ax") \
    read_addr() \

#define read_pc_and_inc \
    echo("movzx eax, word ptr [rdi + 2048]") \
    echo("mov ecx, eax") \
    echo("add ecx, 1") \
    echo("mov word ptr [rdi + 2048], cx") \
    echo("mov word ptr [rdi + 2062], ax") \

#define testInterrupt() \
    echo("mov al, byte ptr [rdi + 2057]") \
    echo("mov cl, byte ptr [rdi + 2081]") \
    echo("or cl, byte ptr [rdi + 2080]") \
    echo("not al") \
    echo("and al, cl") \
    echo("or al, byte ptr [rdi + 2082]") \
    echo("mov byte ptr [rdi + 2083], al") \

#define addClocks() \
    echo("add dword ptr [rdi + 2064], 3") \

// -----------------------------------------------------------------------------------------
#define abs() \
    read_pc(); \
    throw;
#define abx() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define aby() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define iab() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define imm() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define imp() \
    ;
#define izx() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define izy() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define rel() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define zpg() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define zpx() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define zpy() \
    _system->logger->Log("Unimplemented!"); \
    throw;



// -----------------------------------------------------------------------------------------
#define brk() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define ora() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define asl() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define php() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define bpl() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define clc() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define jsr() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define bit() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define rol() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define and_() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define bmi() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define plp() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define sec() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define rti() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define eor() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define lsr() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define pha() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define jmp() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define bvc() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define cli() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define phy() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define rts() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define adc() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define ror() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define pla() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define bvs() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define sei() \
    testInterrupt(); \
    addClocks(); \
    echo("mov byte ptr [rdi + 2057], 1")
#define ply() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define sta() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define sty() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define stx() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define dey() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define txa() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define bcc() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define tya() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define txs() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define ldy() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define lda() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define ldx() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define tay() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define tax() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define bcs() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define clv() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define tsx() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define cpy() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define cmp() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define iny() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define dec() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define dex() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define bne() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define cld() \
    testInterrupt(); \
    addClocks(); \
    echo("mov byte ptr [rdi + 2056], 0");
#define phx() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define cpx() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define sbc() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define inc() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define inx() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define beq() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define sed() \
    _system->logger->Log("Unimplemented!"); \
    throw;
#define plx() \
    _system->logger->Log("Unimplemented!"); \
    throw;


std::string CpuJit::RecodeInstruction(int pc, int* increment) {
    std::stringstream output;
    Disassembly disasm;


//    auto abs = [&]() -> void { output << " $" << hex2 << (int)_system->mem->Read(pc + 2) << hex2 << (int) _system->mem->Read(pc + 1); };
//    auto abx = [&]() -> void { output << " $" << hex2 << (int)_system->mem->Read(pc + 2) << hex2 << (int)_system->mem->Read(pc + 1) << ",x"; };
//    auto aby = [&]() -> void { output << " $" << hex2 << (int)_system->mem->Read(pc + 2) << hex2 << (int)_system->mem->Read(pc + 1) << ",y"; };
//    auto iab = [&]() -> void { output << " ($" << hex2 << (int)_system->mem->Read(pc + 2) << hex2 << (int)_system->mem->Read(pc + 1) << ")"; };
//    auto imm = [&]() -> void { output << " #$" << hex2 << (int)_system->mem->Read(pc + 1); };
//    auto imp = [&]() -> void { output << "       "; };
//    auto izx = [&]() -> void { output << " ($" << hex2 << (int)_system->mem->Read(pc + 1) << ",x)"; };
//    auto izy = [&]() -> void { output << " ($" << hex2 << (int)_system->mem->Read(pc + 1) << "),y"; };
//    auto rel = [&]() -> void { output << " $" << hex4 << (int)(pc + 2 + (int8_t)_system->mem->Read(pc + 1)); };
//    auto zpg = [&]() -> void { output << " $" << hex2 << (int)_system->mem->Read(pc + 1); };
//    auto zpx = [&]() -> void { output << " $" << hex2 << (int)_system->mem->Read(pc + 1) << ",x"; };
//    auto zpy = [&]() -> void { output << " $" << hex2 << (int)_system->mem->Read(pc + 1) << ",y"; };
//
    auto one   = [&]() -> void { *increment = 1; output << hex2 << (int)_system->mem->Read(pc); };
    auto two   = [&]() -> void { *increment = 2; output << hex2 << (int)_system->mem->Read(pc) << " " << hex2 << (int)_system->mem->Read(pc + 1); };
    auto three = [&]() -> void { *increment = 3; output << hex2 << (int)_system->mem->Read(pc) << " " << hex2 << (int)_system->mem->Read(pc + 1) << " " << hex2 << (int)_system->mem->Read(pc + 2); };


    uint8_t opcode = _system->mem->Read(pc);
    switch(opcode) {
        reop(0x00, brk, imm, one);
        reop(0x01, ora, izx, two);
        reop(0x05, ora, zpg, two);
        reop(0x06, asl, zpg, two);
        reop(0x08, php, imp, one);
        reop(0x09, ora, imm, two);
        reop(0x0a, asl, imp, one);
        reop(0x0d, ora, abs, three);
        reop(0x0e, asl, abs, three);
        reop(0x10, bpl, rel, two);
        reop(0x11, ora, izy, two);
        reop(0x15, ora, zpx, two);
        reop(0x16, asl, zpx, two);
        reop(0x18, clc, imp, one);
        reop(0x19, ora, aby, three);
        reop(0x1d, ora, abx, three);
        reop(0x1e, asl, abx, three);
        reop(0x20, jsr, abs, three);
        reop(0x21, and_, izx, two);
        reop(0x24, bit, zpg, two);
        reop(0x25, and_, zpg, two);
        reop(0x26, rol, zpg, two);
        reop(0x28, plp, imp, one);
        reop(0x29, and_, imm, two);
        reop(0x2a, rol, imp, one);
        reop(0x2c, bit, abs, three);
        reop(0x2d, and_, abs, three);
        reop(0x2e, rol, abs, three);
        reop(0x30, bmi, rel, two);
        reop(0x31, and_, izy, two);
        reop(0x35, and_, zpx, two);
        reop(0x36, rol, zpx, two);
        reop(0x38, sec, imp, one);
        reop(0x39, and_, aby, three);
        reop(0x3d, and_, abx, three);
        reop(0x3e, rol, abx, three);
        reop(0x40, rti, imp, one);
        reop(0x41, eor, izx, two);
        reop(0x45, eor, zpg, two);
        reop(0x46, lsr, zpg, two);
        reop(0x48, pha, imp, one);
        reop(0x49, eor, imm, two);
        reop(0x4a, lsr, imp, one);
        reop(0x4c, jmp, abs, three);
        reop(0x4d, eor, abs, three);
        reop(0x4e, lsr, abs, three);
        reop(0x50, bvc, rel, two);
        reop(0x51, eor, izy, two);
        reop(0x55, eor, zpx, two);
        reop(0x56, lsr, zpx, two);
        reop(0x58, cli, imp, one);
        reop(0x59, eor, aby, three);
        reop(0x5a, phy, imp, one);
        reop(0x5d, eor, abx, three);
        reop(0x5e, lsr, abx, three);
        reop(0x60, rts, imp, one);
        reop(0x61, adc, izx, two);
        reop(0x65, adc, zpg, two);
        reop(0x66, ror, zpg, two);
        reop(0x68, pla, imp, one);
        reop(0x69, adc, imm, two);
        reop(0x6a, ror, imp, one);
        reop(0x6c, jmp, iab, three);
        reop(0x6d, adc, abs, three);
        reop(0x6e, ror, abs, three);
        reop(0x70, bvs, rel, two);
        reop(0x71, adc, izy, two);
        reop(0x75, adc, zpx, two);
        reop(0x76, ror, zpx, two);
        reop(0x78, sei, imp, one);
        reop(0x79, adc, aby, three);
        reop(0x7a, ply, imp, one);
        reop(0x7d, adc, abx, three);
        reop(0x7e, ror, abx, three);
        reop(0x81, sta, izx, two);
        reop(0x84, sty, zpg, two);
        reop(0x85, sta, zpg, two);
        reop(0x86, stx, zpg, two);
        reop(0x88, dey, imp, one);
        reop(0x8a, txa, imp, one);
        reop(0x8c, sty, abs, three);
        reop(0x8d, sta, abs, three);
        reop(0x8e, stx, abs, three);
        reop(0x90, bcc, rel, two);
        reop(0x91, sta, izy, two);
        reop(0x94, sty, zpx, two);
        reop(0x95, sta, zpx, two);
        reop(0x96, stx, zpy, two);
        reop(0x98, tya, imp, one);
        reop(0x99, sta, aby, three);
        reop(0x9a, txs, imp, one);
        reop(0x9d, sta, abx, three);
        reop(0xa0, ldy, imm, two);
        reop(0xa1, lda, izx, two);
        reop(0xa2, ldx, imm, two);
        reop(0xa4, ldy, zpg, two);
        reop(0xa5, lda, zpg, two);
        reop(0xa6, ldx, zpg, two);
        reop(0xa8, tay, imp, one);
        reop(0xa9, lda, imm, two);
        reop(0xaa, tax, imp, one);
        reop(0xac, ldy, abs, three);
        reop(0xad, lda, abs, three);
        reop(0xae, ldx, abs, three);
        reop(0xb0, bcs, rel, two);
        reop(0xb1, lda, izy, two);
        reop(0xb4, ldy, zpx, two);
        reop(0xb5, lda, zpx, two);
        reop(0xb6, ldx, zpy, two);
        reop(0xb8, clv, imp, one);
        reop(0xb9, lda, aby, three);
        reop(0xba, tsx, imp, one);
        reop(0xbc, ldy, abx, three);
        reop(0xbd, lda, abx, three);
        reop(0xbe, ldx, aby, three);
        reop(0xc0, cpy, imm, two);
        reop(0xc1, cmp, izx, two);
        reop(0xc4, cpy, zpg, two);
        reop(0xc5, cmp, zpg, two);
        reop(0xc6, dec, zpg, two);
        reop(0xc8, iny, imp, one);
        reop(0xc9, cmp, imm, two);
        reop(0xca, dex, imp, one);
        reop(0xcc, cpy, abs, three);
        reop(0xcd, cmp, abs, three);
        reop(0xce, dec, abs, three);
        reop(0xd0, bne, rel, two);
        reop(0xd1, cmp, izy, two);
        reop(0xd5, cmp, zpx, two);
        reop(0xd6, dec, zpx, two);
        reop(0xd8, cld, imp, one);
        reop(0xd9, cmp, aby, three);
        reop(0xda, phx, imp, one);
        reop(0xdd, cmp, abx, three);
        reop(0xde, dec, abx, three);
        reop(0xe0, cpx, imm, two);
        reop(0xe1, sbc, izx, two);
        reop(0xe4, cpx, zpg, two);
        reop(0xe5, sbc, zpg, two);
        reop(0xe6, inc, zpg, two);
        reop(0xe8, inx, imp, one);
        reop(0xe9, sbc, imm, two);
        reop(0xec, cpx, abs, three);
        reop(0xed, sbc, abs, three);
        reop(0xee, inc, abs, three);
        reop(0xf0, beq, rel, two);
        reop(0xf1, sbc, izy, two);
        reop(0xf5, sbc, zpx, two);
        reop(0xf6, inc, zpx, two);
        reop(0xf8, sed, imp, one);
        reop(0xf9, sbc, aby, three);
        reop(0xfa, plx, imp, one);
        reop(0xfd, sbc, abx, three);
        reop(0xfe, inc, abx, three);

        default:
            disasm = Disassembly();
            output << hex4 << (int)pc;
            setvalue(disasm.address);
            one();
            setvalue(disasm.bytes);
            output << ".db $" << hex2 << (int)opcode;
            setvalue(disasm.assembly);
            //disassembly[pc] = disasm;
            break;
    }

    _system->logger->Log(disasm.address + "  " + disasm.bytes + "  " + disasm.assembly);
    return disasm.address + "  " + disasm.bytes + "  " + disasm.assembly;
}

#undef hex8
#undef hex4
#undef hex2

CpuJit::CpuJit(ISystem *system) : ICpu(system) {
    _system = system;
    _interpreter = new Cpu(system);
}

void CpuJit::Reset() {
    _interpreter->Reset();
    x = 0;
    y = 0;
    a = 0xFF;
    p = 0x04;
    s = 0xFD;
    Address16 pcaddr;
    pcaddr.l = _system->mem->Read(0xFFFC);
    pcaddr.h = _system->mem->Read(0xFFFD);
    pc = pcaddr.w;
    //pc = 0x8000;

    error = false;
    clocks = 1;

    currentBlock.clear(); //reset for new block

    //output setup
    //echo(".type cpuState,@object        # @cpuState");
    //echo(".comm cpuState,2072,8");

}

void CpuJit::Emit(){
    ks_engine *ks;
    ks_err err;
    unsigned char *encode;
    size_t count;
    size_t size;

    err = ks_open(KS_ARCH_X86, KS_MODE_64, &ks);
    if (err != KS_ERR_OK) {
        _system->logger->Log("ERROR: failed on ks_open(), quit\n");
        return;
    }

    std::string output = "";
    for (auto & it : currentBlock) {
        output += it + ";";
    }
    auto assembly = output.c_str();

    ks_option(ks, KS_OPT_SYNTAX, KS_OPT_SYNTAX_INTEL);

    if (ks_asm(ks, assembly, 0, &encode, &size, &count)) {
        printf("ERROR: failed on ks_asm() with count = %lu, error code = %s\n", count, ks_strerror(ks_errno(ks)));
    } else {
        size_t i;

        printf("%s = ", assembly);
        for (i = 0; i < size; i++) {
            printf("%02x ", encode[i]);
        }
        printf("\n");
        printf("Assembled: %lu bytes, %lu statements\n\n", size, count);
        block = mmap(0, size, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
        memcpy(block, encode, size);
        mprotect(block, size, PROT_READ|PROT_WRITE|PROT_EXEC);
    }

    // NOTE: free encode after usage to avoid leaking memory
    ks_free(encode);

    // close Keystone instance when done
    ks_close(ks);

    return;
}

void CpuJit::Cycle() {


    int opcodeSize = 0;
    DecodeInstruction(pc, &opcodeSize, false);
    //preamble();
    RecodeInstruction(pc, &opcodeSize);
    //Use interpreter for everything at the moment
    _interpreter->Cycle();
    a = _interpreter->a;
    p = _interpreter->p;
    pc = _interpreter->pc;
    s = _interpreter->s;
    x = _interpreter->x;
    y = _interpreter->y;
    cycle = _interpreter->cycle;
    clocks = _interpreter->clocks;

    echo("ret");
    Emit();
    Exec();
//    int opcodeSize = 0;
//    while (pc<=0xFFFF) {
//        DecodeInstruction(pc, &opcodeSize, false);
//        RecodeInstruction(pc, &opcodeSize);
//        pc += opcodeSize;
//    }
}

bool CpuJit::Interrupt() {
    bool inter =_interpreter->Interrupt();
    a = _interpreter->a;
    p = _interpreter->p;
    pc = _interpreter->pc;
    s = _interpreter->s;
    x = _interpreter->x;
    y = _interpreter->y;
    cycle = _interpreter->cycle;
    clocks = _interpreter->clocks;
    return inter;
}

void CpuJit::Apu(bool line) {
    _interpreter->Apu(line);
}

void CpuJit::Irq(bool line) {
    _interpreter->Irq(line);
}

void CpuJit::Nmi(bool line) {
    _interpreter->Nmi(line);
}

void CpuJit::Exec() {
    cpuState.ram[0] = 0xF0;
    cpuState.ram[1] = 0x0D;
    //jump to block
    void (*exec)(state *cpuState) = (void(*)(state *cpuState))block;
    exec(&cpuState);
}
