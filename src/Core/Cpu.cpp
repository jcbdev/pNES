//
// Created by James Barker on 16/11/2016.
//

#include "Cpu.h"
#include "Memory.h"
#include "Ppu.h"
#include "../Helpers/Logger.h"
#include "Debug.h"

ICpu::ICpu(ISystem* system) {
    _system = system;
}

Cpu::Cpu(ISystem* system) : ICpu::ICpu(system) {
    _interruptPending = false;
    _nmi = false;
    _nmiPending = false;
    _irq = false;
    _apu = false;
}

uint8_t Cpu::_readPcAndInc() {
    return _read(pc++);
}

void Cpu::_addClocks() {
    clocks += 3;
}

uint8_t Cpu::_read(uint16_t addr) {
    _addClocks();
    uint8_t data = _system->mem->Read(addr);
    if (_dmaPending){
        _dmaPending = false;

        _system->ppu->WriteDMA(_dmaPage);
    }
    return data;
}

void Cpu::_write(uint16_t addr, uint8_t data) {
    //_addClocks();
    if (addr == 0x4014){
        _dmaPending = true;
        _dmaPage = data;
        //return?
    }

    _system->mem->Write(addr, data);
}

void Cpu::_writeSp(uint8_t data){
    _addClocks();
    //_system->logger->Log("WriteSP: " + std::to_string(data) + "    pc:" + std::to_string(pc) + "    s:" + std::to_string(s));
    _write((uint16_t)0x0100 | s--, data);
}

uint8_t Cpu::_readZp(uint8_t zp){
    _addClocks();
    return _system->mem->ReadZP(zp);
};

void Cpu::_writeZp(uint8_t zp, uint8_t data) {
    //_addClocks();
    _system->mem->WriteZP(zp, data);
}

uint8_t Cpu::_readSp(){
    uint16_t val = _read(0x0100 | ++s);
    //_system->logger->Log("ReadSP: " + std::to_string(val) + "    pc:" + std::to_string(pc) + "    s:" + std::to_string(s-1));
    return val;
}

void Cpu::Reset() {
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

    _nmi = false;
    _nmiPending = false;
    _irq = false;
    _apu = false;
    _dmaPending = false;
    _dmaPage = 0x00;

    error = false;
    clocks = 1;
}



void Cpu::_printClockDrift(uint8_t opcode) {
    int16_t drift = 0;
    std::string prefix = "";

#define d(byte, pfx, c) \
    case byte: \
        prefix = #pfx; \
        drift = (clocks/3) - (c); \
        break

    switch(opcode){
        d(0x00, brk, 7);
        d(0x01, ora, 6);
        d(0x05, ora, 3);
        d(0x06, asl, 5);
        d(0x08, php, 3);
        d(0x09, ora, 2);
        d(0x0a, asl, 3);
        d(0x0d, ora, 4);
        d(0x0e, asl, 6);
        d(0x10, bpl, 3);
        d(0x11, ora, 5);
        d(0x15, ora, 4);
        d(0x16, asl, 6);
        d(0x18, clc, 2);
        d(0x19, ora, 4);
        d(0x1d, ora, 4);
        d(0x1e, asl, 7);
        d(0x20, jsr, 6);
        d(0x21, and, 6);
        d(0x24, bit, 3);
        d(0x25, and, 3);
        d(0x26, rol, 5);
        d(0x28, plp, 4);
        d(0x29, and, 2);
        d(0x2a, rol, 2);
        d(0x2c, bit, 4);
        d(0x2d, and, 4);
        d(0x2e, rol, 6);
        d(0x30, bmi, 3);
        d(0x31, and, 5);
        d(0x35, and, 4);
        d(0x36, rol, 6);
        d(0x38, sec, 2);
        d(0x39, and, 4);
        d(0x3d, and, 4);
        d(0x3e, rol, 7);
        d(0x40, rti, 6);
        d(0x41, eor, 6);
        d(0x45, eor, 3);
        d(0x46, lsr, 5);
        d(0x48, pha, 3);
        d(0x49, eor, 2);
        d(0x4a, lsr, 2);
        d(0x4c, jmp, 3);
        d(0x4d, eor, 4);
        d(0x4e, lsr, 6);
        d(0x50, bvc, 3);
        d(0x51, eor, 5);
        d(0x55, eor, 4);
        d(0x56, lsr, 6);
        d(0x58, cli, 2);
        d(0x59, eor, 4);
        d(0x5a, phy, 2);
        d(0x5d, eor, 4);
        d(0x5e, lsr, 7);
        d(0x60, rts, 6);
        d(0x61, adc, 6);
        d(0x65, adc, 3);
        d(0x66, ror, 5);
        d(0x68, pla, 4);
        d(0x69, adc, 2);
        d(0x6a, ror, 2);
        d(0x6c, jmp, 5);
        d(0x6d, adc, 4);
        d(0x6e, ror, 6);
        d(0x70, bvs, 3);
        d(0x71, adc, 5);
        d(0x75, adc, 4);
        d(0x76, ror, 6);
        d(0x78, sei, 2);
        d(0x79, adc, 4);
        d(0x7a, ply, 2);
        d(0x7d, adc, 4);
        d(0x7e, ror, 7);
        d(0x81, sta, 6);
        d(0x84, sty, 3);
        d(0x85, sta, 3);
        d(0x86, stx, 3);
        d(0x88, dey, 2);
        d(0x8a, txa, 2);
        d(0x8c, sty, 4);
        d(0x8d, sta, 4);
        d(0x8e, stx, 4);
        d(0x90, bcc, 3);
        d(0x91, sta, 6);
        d(0x94, sty, 4);
        d(0x95, sta, 4);
        d(0x96, stx, 4);
        d(0x98, tya, 2);
        d(0x99, sta, 5);
        d(0x9a, txs, 2);
        d(0x9d, sta, 5);
        d(0xa0, ldy, 2);
        d(0xa1, lda, 6);
        d(0xa2, ldx, 2);
        d(0xa4, ldy, 3);
        d(0xa5, lda, 3);
        d(0xa6, ldx, 3);
        d(0xa8, tay, 2);
        d(0xa9, lda, 2);
        d(0xaa, tax, 2);
        d(0xac, ldy, 4);
        d(0xad, lda, 4);
        d(0xae, ldx, 4);
        d(0xb0, bcs, 3);
        d(0xb1, lda, 5);
        d(0xb4, ldy, 4);
        d(0xb5, lda, 4);
        d(0xb6, ldx, 4);
        d(0xb8, clv, 2);
        d(0xb9, lda, 4);
        d(0xba, tsx, 2);
        d(0xbc, ldy, 4);
        d(0xbd, lda, 4);
        d(0xbe, ldx, 4);
        d(0xc0, cpy, 2);
        d(0xc1, cmp, 6);
        d(0xc4, cpy, 3);
        d(0xc5, cmp, 3);
        d(0xc6, dec, 5);
        d(0xc8, iny, 2);
        d(0xc9, cmp, 2);
        d(0xca, dex, 2);
        d(0xcc, cpy, 4);
        d(0xcd, cmp, 4);
        d(0xce, dec, 6);
        d(0xd0, bne, 3);
        d(0xd1, cmp, 5);
        d(0xd5, cmp, 4);
        d(0xd6, dec, 6);
        d(0xd8, cld, 2);
        d(0xd9, cmp, 4);
        d(0xda, phx, 2);
        d(0xdd, cmp, 4);
        d(0xde, dec, 7);
        d(0xe0, cpx, 2);
        d(0xe1, sbc, 6);
        d(0xe4, cpx, 3);
        d(0xe5, sbc, 3);
        d(0xe6, inc, 5);
        d(0xe8, inx, 2);
        d(0xe9, sbc, 2);
        d(0xec, cpx, 4);
        d(0xed, sbc, 4);
        d(0xee, inc, 6);
        d(0xf0, beq, 3);
        d(0xf1, sbc, 5);
        d(0xf5, sbc, 4);
        d(0xf6, inc, 6);
        d(0xf8, sed, 2);
        d(0xf9, sbc, 4);
        d(0xfa, plx, 2);
        d(0xfd, sbc, 4);
        d(0xfe, inc, 7);

        //illegals
        d(0x04, ndill, 2);
        d(0x0C, ndill, 2);
        d(0x14, ndill, 2);
        d(0x1A, ndill, 2);
        d(0x1C, ndill, 2);
        d(0x34, ndill, 2);
        d(0x3A, ndill, 2);
        d(0x3C, ndill, 2);
        d(0x44, ndill, 2);
        d(0x54, ndill, 2);
        d(0x5C, ndill, 2);
        d(0x64, ndill, 2);
        d(0x6B, arrill, 2);
        d(0x74, ndill, 2);
        d(0x7C, ndill, 2);
        d(0x80, ndill, 2);
        d(0x82, ndill, 2);
        d(0x89, ndill, 2);
        d(0xC2, ndill, 2);
        d(0xD4, ndill, 2);
        d(0xDC, ndill, 2);
        d(0xE2, ndill, 2);
        d(0xeb, sbcill, 2);
        d(0xF4, ndill, 2);
        d(0xFC, ndill, 2);

        default: error = true; break;
    }
#undef d
    auto idrift = drift;
    if (drift > 7) {
        drift = drift - 513;
        if ((_system->cpu->totalClocks%2) == 0)
            drift--;
    }
    if (_paged)
        drift--;
    if (drift != 0)
    {
        std::stringstream output;
#define hex2 std::setfill('0') << std::setw(2) << std::hex
        output << hex2 << (int)opcode << "\t" << prefix << "\t" << (int)drift;
#undef hex2
        _system->logger->Log(output.str());
    }
}

void Cpu::Cycle() {
    _paged = false;

    int size;
    std::stringstream traceLog;
    traceLog << _system->debug->Decode(pc, &size, false);
    traceLog << "    a:" << _tohex(a) << ", x:" << _tohex(x) << ", y:" << _tohex(y)
             << ", sp:" << _tohex(s) << ", p:" << _tohex((uint8_t)p);
    _system->logger->Log("");
    _system->logger->Log(traceLog.str());
    _system->logger->Log("----------------------------------------------------------------------------------------------------");
    uint8_t opcode = _readPcAndInc();
    //_system->logger->Log(std::to_string(opcode));


    switch(opcode){
        //ADC
        case 0x69: Read(&Cpu::Immediate, &Cpu::Adc); break;
        case 0x65: Read(&Cpu::Zeropage, &Cpu::Adc); break;
        case 0x75: Read(&Cpu::ZeropageX, &Cpu::Adc); break;
        case 0x6D: Read(&Cpu::Absolute, &Cpu::Adc); break;
        case 0x7D: Read(&Cpu::AbsoluteX, &Cpu::Adc); break;
        case 0x79: Read(&Cpu::AbsoluteY, &Cpu::Adc); break;
        case 0x61: Read(&Cpu::IndirectX, &Cpu::Adc); break;
        case 0x71: Read(&Cpu::IndirectY, &Cpu::Adc); break;

        //And
        case 0x29: Read(&Cpu::Immediate, &Cpu::And); break;
        case 0x25: Read(&Cpu::Zeropage, &Cpu::And); break;
        case 0x35: Read(&Cpu::ZeropageX, &Cpu::And); break;
        case 0x2D: Read(&Cpu::Absolute, &Cpu::And); break;
        case 0x3D: Read(&Cpu::AbsoluteX, &Cpu::And); break;
        case 0x39: Read(&Cpu::AbsoluteY, &Cpu::And); break;
        case 0x21: Read(&Cpu::IndirectX, &Cpu::And); break;
        case 0x31: Read(&Cpu::IndirectY, &Cpu::And); break;

        //Asl
        case 0x0A: AslAccumulator(); break;
        case 0x06: Rmw(&Cpu::Zeropage, &Cpu::Asl); break;
        case 0x16: Rmw(&Cpu::ZeropageX, &Cpu::Asl); break;
        case 0x0E: Rmw(&Cpu::Absolute, &Cpu::Asl); break;
        case 0x1E: Rmw(&Cpu::AbsoluteX, &Cpu::Asl); break;

        //Bit
        case 0x24: Read(&Cpu::Zeropage, &Cpu::Bit); break;
        case 0x2C: Read(&Cpu::Absolute, &Cpu::Bit); break;

        //Branch
        case 0x10: Branch(!p.n); break;
        case 0x30: Branch(p.n); break;
        case 0x50: Branch(!p.v); break;
        case 0x70: Branch(p.v); break;
        case 0x90: Branch(!p.c); break;
        case 0xB0: Branch(p.c); break;
        case 0xD0: Branch(!p.z); break;
        case 0xF0: Branch(p.z); break;

        //Brk
        case 0x00: Brk(); break;

        //Cmp
        case 0xC9: Read(&Cpu::Immediate, &Cpu::Cmp); break;
        case 0xC5: Read(&Cpu::Zeropage, &Cpu::Cmp); break;
        case 0xD5: Read(&Cpu::ZeropageX, &Cpu::Cmp); break;
        case 0xCD: Read(&Cpu::Absolute, &Cpu::Cmp); break;
        case 0xDD: Read(&Cpu::AbsoluteX, &Cpu::Cmp); break;
        case 0xD9: Read(&Cpu::AbsoluteY, &Cpu::Cmp); break;
        case 0xC1: Read(&Cpu::IndirectX, &Cpu::Cmp); break;
        case 0xD1: Read(&Cpu::IndirectY, &Cpu::Cmp); break;

        //Cpx
        case 0xE0: Read(&Cpu::Immediate, &Cpu::Cpx); break;
        case 0xE4: Read(&Cpu::Zeropage, &Cpu::Cpx); break;
        case 0xEC: Read(&Cpu::Absolute, &Cpu::Cpx); break;

        //Cpy
        case 0xC0: Read(&Cpu::Immediate, &Cpu::Cpy); break;
        case 0xC4: Read(&Cpu::Zeropage, &Cpu::Cpy); break;
        case 0xCC: Read(&Cpu::Absolute, &Cpu::Cpy); break;

        //Dec
        case 0xC6: Rmw(&Cpu::Zeropage, &Cpu::Dec); break;
        case 0xD6: Rmw(&Cpu::ZeropageX, &Cpu::Dec); break;
        case 0xCE: Rmw(&Cpu::Absolute, &Cpu::Dec); break;
        case 0xDE: Rmw(&Cpu::AbsoluteX, &Cpu::Dec); break;
        case 0xCA: Dex(); break;
        case 0x88: Dey(); break;

        //Eor
        case 0x49: Read(&Cpu::Immediate, &Cpu::Eor); break;
        case 0x45: Read(&Cpu::Zeropage, &Cpu::Eor); break;
        case 0x55: Read(&Cpu::ZeropageX, &Cpu::Eor); break;
        case 0x4D: Read(&Cpu::Absolute, &Cpu::Eor); break;
        case 0x5D: Read(&Cpu::AbsoluteX, &Cpu::Eor); break;
        case 0x59: Read(&Cpu::AbsoluteY, &Cpu::Eor); break;
        case 0x41: Read(&Cpu::IndirectX, &Cpu::Eor); break;
        case 0x51: Read(&Cpu::IndirectY, &Cpu::Eor); break;

        //Processor Status
        case 0x18: FlagClear(p.c); break; //CLC
        case 0x38: FlagSet(p.c); break; //SEC
        case 0x58: FlagClear(p.i); break; //CLI
        case 0x78: FlagSet(p.i); break; //SEI
        case 0xB8: FlagClear(p.v); break; //CLV
        case 0xD8: FlagClear(p.d); break; //CLD
        case 0xF8: FlagSet(p.d); break; //SED

        //Inc
        case 0xE6: Rmw(&Cpu::Zeropage, &Cpu::Inc); break;
        case 0xF6: Rmw(&Cpu::ZeropageX, &Cpu::Inc); break;
        case 0xEE: Rmw(&Cpu::Absolute, &Cpu::Inc); break;
        case 0xFE: Rmw(&Cpu::AbsoluteX, &Cpu::Inc); break;
        case 0xE8: Inx(); break;
        case 0xC8: Iny(); break;

        //Illegal
        case 0x04: IllegalNopZeroPage(); break;
        case 0x0C: IllegalNopAbsolute(); break;
        case 0x14: IllegalNopZeroPageX(); break;
        case 0x1A: IllegalNopImplied(); break;
        case 0x1C: IllegalNopAbsoluteX(); break;
        case 0x34: IllegalNopZeroPageX(); break;
        case 0x3A: IllegalNopImplied(); break;
        case 0x3C: IllegalNopAbsoluteX(); break;
        case 0x44: IllegalNopZeroPage(); break;
        case 0x54: IllegalNopZeroPageX(); break;
        case 0x5A: IllegalNopImplied(); break;
        case 0x5C: IllegalNopAbsoluteX(); break;
        case 0x64: IllegalNopZeroPage(); break;
        case 0x6B: IllegalArrImmediate(); break;
        case 0x74: IllegalNopZeroPageX(); break;
        case 0x7A: IllegalNopImplied(); break;
        case 0x7C: IllegalNopAbsoluteX(); break;
        case 0x80: IllegalNopAbsolute(); break;
        case 0x82: IllegalNopImmediate(); break;
        case 0x89: IllegalNopImmediate(); break;
        case 0xC2: IllegalNopImmediate(); break;

        case 0xD4: IllegalNopZeroPageX(); break;
        case 0xDA: IllegalNopImplied(); break;
        case 0xDC: IllegalNopAbsoluteX(); break;
        case 0xE2: IllegalNopImmediate(); break;
        case 0xeb: Read(&Cpu::Immediate, &Cpu::Sbc); break;
        case 0xF4: IllegalNopZeroPageX(); break;
        case 0xFA: IllegalNopImplied(); break;
        case 0xFC: IllegalNopAbsoluteX(); break;

        //Jmp
        case 0x4C: JmpAbsolute(); break;
        case 0x6C: JmpIndirect(); break;

        //Jsr
        case 0x20: Jsr(); break;

        //Lda
        case 0xA9: Read(&Cpu::Immediate, &Cpu::Lda); break;
        case 0xA5: Read(&Cpu::Zeropage, &Cpu::Lda); break;
        case 0xB5: Read(&Cpu::ZeropageX, &Cpu::Lda); break;
        case 0xAD: Read(&Cpu::Absolute, &Cpu::Lda); break;
        case 0xBD: Read(&Cpu::AbsoluteX, &Cpu::Lda); break;
        case 0xB9: Read(&Cpu::AbsoluteY, &Cpu::Lda); break;
        case 0xA1: Read(&Cpu::IndirectX, &Cpu::Lda); break;
        case 0xB1: Read(&Cpu::IndirectY, &Cpu::Lda); break;

        //Ldx
        case 0xA2: Read(&Cpu::Immediate, &Cpu::Ldx); break;
        case 0xA6: Read(&Cpu::Zeropage, &Cpu::Ldx); break;
        case 0xB6: Read(&Cpu::ZeropageY, &Cpu::Ldx); break;
        case 0xAE: Read(&Cpu::Absolute, &Cpu::Ldx); break;
        case 0xBE: Read(&Cpu::AbsoluteY, &Cpu::Ldx); break;

        //Ldy
        case 0xA0: Read(&Cpu::Immediate, &Cpu::Ldy); break;
        case 0xA4: Read(&Cpu::Zeropage, &Cpu::Ldy); break;
        case 0xB4: Read(&Cpu::ZeropageY, &Cpu::Ldy); break;
        case 0xAC: Read(&Cpu::Absolute, &Cpu::Ldy); break;
        case 0xBC: Read(&Cpu::AbsoluteY, &Cpu::Ldy); break;

        //Lsr
        case 0x4A: LsrAccumulator(); break;
        case 0x46: Read(&Cpu::Zeropage, &Cpu::Lsr); break;
        case 0x56: Read(&Cpu::ZeropageX, &Cpu::Lsr); break;
        case 0x4E: Read(&Cpu::Absolute, &Cpu::Lsr); break;
        case 0x5E: Read(&Cpu::AbsoluteX, &Cpu::Lsr); break;

        //Nop
        case 0xEA: Cpu::Nop(); break;

        //Ora
        case 0x09: Read(&Cpu::Immediate, &Cpu::Ora); break;
        case 0x05: Read(&Cpu::Zeropage, &Cpu::Ora); break;
        case 0x15: Read(&Cpu::ZeropageX, &Cpu::Ora); break;
        case 0x0D: Read(&Cpu::Absolute, &Cpu::Ora); break;
        case 0x1D: Read(&Cpu::AbsoluteX, &Cpu::Ora); break;
        case 0x19: Read(&Cpu::AbsoluteY, &Cpu::Ora); break;
        case 0x01: Read(&Cpu::IndirectX, &Cpu::Ora); break;
        case 0x11: Read(&Cpu::IndirectY, &Cpu::Ora); break;

        //Push
        case 0x48: Pha(); break;
        case 0x08: Php(); break;

        //Pla
        case 0x68: Pla(); break;
        case 0x28: Plp(); break;

        //Rol
        case 0x2A: RolAccumulator(); break;
        case 0x26: Rmw(&Cpu::Zeropage, &Cpu::Rol); break;
        case 0x36: Rmw(&Cpu::ZeropageX, &Cpu::Rol); break;
        case 0x2E: Rmw(&Cpu::Absolute, &Cpu::Rol); break;
        case 0x3E: Rmw(&Cpu::AbsoluteX, &Cpu::Rol); break;

        //Ror
        case 0x6A: RorAccumulator(); break;
        case 0x66: Rmw(&Cpu::Zeropage, &Cpu::Ror); break;
        case 0x76: Rmw(&Cpu::ZeropageX, &Cpu::Ror); break;
        case 0x6E: Rmw(&Cpu::Absolute, &Cpu::Ror); break;
        case 0x7E: Rmw(&Cpu::AbsoluteX, &Cpu::Ror); break;

        //Rti
        case 0x40: Rti(); break;

        //Rts
        case 0x60: Rts(); break;

        //Sbc
        case 0xE9: Read(&Cpu::Immediate, &Cpu::Sbc); break;
        case 0xE5: Read(&Cpu::Zeropage, &Cpu::Sbc); break;
        case 0xF5: Read(&Cpu::ZeropageX, &Cpu::Sbc); break;
        case 0xED: Read(&Cpu::Absolute, &Cpu::Sbc); break;
        case 0xFD: Read(&Cpu::AbsoluteX, &Cpu::Sbc); break;
        case 0xF9: Read(&Cpu::AbsoluteY, &Cpu::Sbc); break;
        case 0xE1: Read(&Cpu::IndirectX, &Cpu::Sbc); break;
        case 0xF1: Read(&Cpu::IndirectY, &Cpu::Sbc); break;

        //Sta
        case 0x85: Store(&Cpu::Zeropage, &Cpu::Sta); break;
        case 0x95: Store(&Cpu::ZeropageX, &Cpu::Sta); break;
        case 0x8D: Store(&Cpu::Absolute, &Cpu::Sta); break;
        case 0x9D: Store(&Cpu::AbsoluteX, &Cpu::Sta); break;
        case 0x99: Store(&Cpu::AbsoluteY, &Cpu::Sta); break;
        case 0x81: Store(&Cpu::IndirectX, &Cpu::Sta); break;
        case 0x91: Store(&Cpu::IndirectY, &Cpu::Sta); break;

        //Stx
        case 0x86: Store(&Cpu::Zeropage, &Cpu::Stx); break;
        case 0x96: Store(&Cpu::ZeropageY, &Cpu::Stx); break;
        case 0x8E: Store(&Cpu::Absolute, &Cpu::Stx); break;

        //Sty
        case 0x84: Store(&Cpu::Zeropage, &Cpu::Sty); break;
        case 0x94: Store(&Cpu::ZeropageX, &Cpu::Sty); break;
        case 0x8C: Store(&Cpu::Absolute, &Cpu::Sty); break;

        //Transfer
        case 0xAA: Cpu::Tax(); break;
        case 0x8A: Cpu::Txa(); break;
        case 0xA8: Cpu::Tay(); break;
        case 0x98: Cpu::Tya(); break;
        case 0x9A: Cpu::Txs(); break;
        case 0xBA: Cpu::Tsx(); break;

        default: error = true; break;
    }
    _system->totalClocks += (clocks/3);
    totalClocks++;
    _printClockDrift(opcode);
    //_system->logger->Log(std::to_string(clocks/3));
}

void Cpu::Adc() {
    int32_t new_a = a + p.c + _val;
    p.v = ~(a ^ _val) & (a ^ new_a) & 0x80;
    p.c = new_a > 0xff;
    p.n = new_a & 0x80;
    p.z = ((uint8_t)new_a == 0);
    a = new_a;
}

void Cpu::And() {
    a &= _val;
    p.n = (a & 0x80);
    p.z = (a == 0);
}

void Cpu::Asl() {
    _addClocks();
    p.c = _val & 0x80;
    _val <<= 1;
    p.n = (_val & 0x80);
    p.z = (_val == 0);
}

void Cpu::AslAccumulator(){
    _addClocks();
    _addClocks();
    _testInterrupt();
    p.c = a & 0x80;
    a <<= 1;
    p.n = (a & 0x80);
    p.z = (a == 0);
}

void Cpu::Bit(){
    p.z = ((_val & a) == 0);
    p.n = (_val & 0x80);
    p.v = (_val & 0x40);
}

void Cpu::Branch(bool condition){
    if (!condition) _testInterrupt();
    int8_t branchval = _readPcAndInc();
    _addClocks();
    if (condition) {
        uint16_t val16 = pc + (int8_t)branchval;
        _paged = _system->mem->PageIfRequired(pc, val16);
        if (_paged)
            _addClocks();
        _testInterrupt();
        pc = val16;
    }
}

void Cpu::Brk(){
    _readPcAndInc();
    _writeSp(pc >> 8);
    _writeSp(pc >> 0);
    _writeSp(p | 0x30);
    _addr16.l = _read(0xfffe);
    p.i = 1;
    p.d = 0;
    _testInterrupt();
    _addr16.h = _read(0xffff);
    pc = _addr16.w;
}

void Cpu::Cmp(){
    signed r = a - _val;
    p.n = (r & 0x80);
    p.z = (uint8_t)(r == 0);
    p.c = (r >= 0);
}

void Cpu::Cpx(){
    signed r = x - _val;
    p.n = (r & 0x80);
    p.z = (uint8_t)(r == 0);
    p.c = (r >= 0);
}

void Cpu::Cpy(){
    signed r = y - _val;
    p.n = (r & 0x80);
    p.z = (uint8_t)(r == 0);
    p.c = (r >= 0);
}

void Cpu::Dec(){
    _addClocks();
    _testInterrupt();
    _val--;
    p.n = (_val & 0x80);
    p.z = (_val == 0);
}

void Cpu::Dex(){
    _addClocks();
    x--;
    p.n = (x & 0x80);
    p.z = (x == 0);
};

void Cpu::Dey(){
    _addClocks();
    y--;
    p.n = (y & 0x80);
    p.z = (y == 0);
}

void Cpu::Eor(){
    a ^= _val;
    p.n = (a & 0x80);
    p.z = (a == 0);
}

void Cpu::FlagClear(bool &flag){
    _testInterrupt();
    _addClocks();
    flag = false;
}

void Cpu::FlagSet(bool &flag){
    _testInterrupt();
    _addClocks();
    flag = true;
}

void Cpu::Inc(){
    _addClocks();
    _testInterrupt();
    _val++;
    p.n = (_val & 0x80);
    p.z = (_val == 0);
}

void Cpu::Inx(){
    _addClocks();
    x++;
    p.n = (x & 0x80);
    p.z = (x == 0);
}

void Cpu::Iny(){
    _addClocks();
    y++;
    p.n = (y & 0x80);
    p.z = (y == 0);
}

void Cpu::JmpAbsolute(){
    _addr16.l = _readPcAndInc();
    _testInterrupt();
    _addr16.h = _readPcAndInc();
    pc = _addr16.w;
}

void Cpu::JmpIndirect() {
    _addr16.l = _readPcAndInc();
    _addr16.h = _readPcAndInc();
    Address16 indirectAddr;
    indirectAddr.l = _read(_addr16.w); _addr16.l++;
    _testInterrupt();
    indirectAddr.h = _read(_addr16.w); _addr16.l++;
    pc = indirectAddr.w;
}

void Cpu::Jsr(){
    _addr16.l = _readPcAndInc();
    _addr16.h = _readPcAndInc();
    _addClocks();
    pc--;
    _writeSp((uint8_t)(pc >> 8));
    _testInterrupt();
    _writeSp((uint8_t)(pc >> 0));
    pc = _addr16.w;
}

void Cpu::Lda() {
    a = _val;
    p.n = (a & 0x80);
    p.z = (a == 0);
}

void Cpu::Ldx() {
    x = _val;
    p.n = x & 0x80;
    p.z = x == 0;
}

void Cpu::Ldy() {
    y = _val;
    p.n = (y & 0x80);
    p.z = (y == 0);
}

void Cpu::Lsr() {
    p.c = _val & 0x01;
    _val >>= 1;
    p.n = (_val & 0x80);
    p.z = (_val == 0);
    _addClocks();
    _addClocks();
}

void Cpu::LsrAccumulator() {
    _addClocks();
    p.c = a & 0x01;
    a >>= 1;
    p.n = (a & 0x80);
    p.z = (a == 0);
}

void Cpu::Nop(){
    _testInterrupt();
    _addClocks();
}

void Cpu::Ora(){
    a |= _val;
    p.n = (a & 0x80);
    p.z = (a == 0);
}

void Cpu::Pha(){
    _addClocks();
    _testInterrupt();
    _writeSp(a);
}

void Cpu::Php(){
    _addClocks();
    _testInterrupt();
    _writeSp(p | 0x30);
}

void Cpu::Pla(){
    _addClocks();
    _addClocks();
    _testInterrupt();
    a = _readSp();
    p.n = (a & 0x80);
    p.z = (a == 0);
}

void Cpu::Plp(){
    _addClocks();
    _addClocks();
    _testInterrupt();
    p = _readSp();
}

void Cpu::RolAccumulator(){
    uint8_t carry = (uint8_t)p.c;
    p.c = a & 0x80;
    a = (a << 1) | carry;
    p.n = (a & 0x80);
    p.z = (a == 0);
    _addClocks();
}

void Cpu::Rol() {
    _addClocks();
    uint8_t carry = (uint8_t)p.c;
    p.c = _val & 0x80;
    _val = (_val << 1) | carry;
    p.n = (_val & 0x80);
    p.z = (_val == 0);
}

void Cpu::RorAccumulator() {
    _addClocks();
    uint8_t carry = (uint8_t)p.c << 7;
    p.c = a & 0x01;
    a = carry | (a >> 1);
    p.n = (a & 0x80);
    p.z = (a == 0);
}

void Cpu::Ror() {
    _addClocks();
    uint8_t carry = (uint8_t)p.c << 7;
    p.c = _val & 0x01;
    _val = carry | (_val >> 1);
    p.n = (_val & 0x80);
    p.z = (_val == 0);
}

void Cpu::Rti() {
    _addClocks();
    _addClocks();
    p = _readSp();
    _addr16.l = _readSp();
    _testInterrupt();
    _addr16.h = _readSp();
    pc = _addr16.w;
}

void Cpu::Rts() {
    _addClocks();
    _addClocks();
    _addr16.l = _readSp();
    _addr16.h = _readSp();
    _testInterrupt();
    _addClocks();
    pc = ++_addr16.w;
}

void Cpu::Sbc() {
    _val ^= 0xff;
    int16_t new_a = a + p.c + _val;
    p.v = ~(a ^ _val) & (a ^ new_a) & 0x80;
    p.c = new_a > 0xff;
    p.n = new_a & 0x80;
    p.z = ((uint8_t)new_a == 0);
    a = new_a;
}

void Cpu::Sta() {
    _val = a;
}

void Cpu::Stx() {
    _val = x;
}

void Cpu::Sty(){
    _val = y;
}

void Cpu::Tax() {
    _testInterrupt();
    _addClocks();
    x = a;
    p.n = (x & 0x80);
    p.z = (x == 0);
}

void Cpu::Txa() {
    _testInterrupt();
    _addClocks();
    a = x;
    p.n = (a & 0x80);
    p.z = (a == 0);
}

void Cpu::Tay() {
    _testInterrupt();
    _addClocks();
    y = a;
    p.n = (y & 0x80);
    p.z = (y == 0);
}

void Cpu::Tya() {
    _testInterrupt();
    _addClocks();
    a = y;
    p.n = (a & 0x80);
    p.z = (a == 0);
}

void Cpu::Txs() {
    _testInterrupt();
    _addClocks();
    s = x;
}

void Cpu::Tsx() {
    _testInterrupt();
    _addClocks();
    x = s;
    p.n = (x & 0x80);
    p.z = (x == 0);
}

void Cpu::IllegalNopZeroPage(){
    _val = _readPcAndInc();
    _testInterrupt();
    _readZp(_val);
}

void Cpu::IllegalNopAbsolute(){
    _addr16.l = _readPcAndInc();
    _addr16.h = _readPcAndInc();
    _testInterrupt();
    _addClocks();
}

void Cpu::IllegalNopZeroPageX(){
    _val = _readPcAndInc();
    _readZp(_val);
    _readZp(_val + x);
}

void Cpu::IllegalNopImplied(){
    _testInterrupt();
    _addClocks();
}

void Cpu::IllegalNopAbsoluteX(){
    _addr16.l = _readPcAndInc();
    _addr16.h = _readPcAndInc();
    _paged = _system->mem->PageIfRequired(_addr16.w, _addr16.w + x);
    if (_paged) _addClocks();
    _testInterrupt();
    _addClocks();
}

void Cpu::IllegalArrImmediate() {
    _testInterrupt();
    _val = _readPcAndInc();
    a &= _val;
    a = (p.c << 7) | (a >> 1);
    p.n = (a & 0x80);
    p.z = (a == 0);
    p.c = (a & 0x40);
    p.v = p.c ^ ((a >> 5) & 1);
}

void Cpu::IllegalNopImmediate() {
    _testInterrupt();
    _addClocks();
}

void Cpu::Immediate(void (Cpu::*opcode)(), bool rmw, bool write) {
    _testInterrupt();
    _val = _readPcAndInc();
    (this->*opcode)();
}

void Cpu::Zeropage(void (Cpu::*opcode)(), bool rmw, bool write) {
    uint8_t zp = _readPcAndInc();
    if (!rmw && !write) _testInterrupt();
    _val = _readZp(zp);
    if (rmw) {
        if (!write) _testInterrupt();
        _writeZp(zp, _val);
        _addClocks();
    }
    (this->*opcode)();
    if (write) {
        _testInterrupt();
        _writeZp(zp, _val);
    }
}

void Cpu::ZeropageX(void (Cpu::*opcode)(), bool rmw, bool write) {
    uint8_t zp = _readPcAndInc();
    if (!rmw && !write) _testInterrupt();
    _val = _readZp(zp + x);
    _addClocks();
    if (rmw) {
        if (!write) _testInterrupt();
        _writeZp(zp + x, _val);
    }
    (this->*opcode)();
    if (write) {
        _testInterrupt();
        _writeZp(zp + x, _val);
    }
}

void Cpu::ZeropageY(void (Cpu::*opcode)(), bool rmw, bool write) {
    uint8_t zp = _readPcAndInc();
    if (!rmw && !write) _testInterrupt();
    _val = _readZp(zp + y);
    if (rmw) {
        if (!write) _testInterrupt();
        _writeZp(zp + y, _val);
    }
    (this->*opcode)();
    if (write) {
        _testInterrupt();
        _writeZp(zp + y, _val);
    }
}

void Cpu::IndirectX(void (Cpu::*opcode)(), bool rmw, bool write) {
    uint8_t zp = _readPcAndInc();
    _readZp(zp);
    _addr16.l = _readZp(zp++ + x);
    _addr16.h = _readZp(zp++ + x);
    if (!rmw && !write) _testInterrupt();
    if (!write) _val = _read(_addr16.w);
    if (rmw) {
        if (!write) _testInterrupt();
        _write(_addr16.w, _val);
    }
    (this->*opcode)();
    if (write) {
        _testInterrupt();
        _writeZp(_addr16.w, _val);
        _addClocks();
        _addClocks();
    }
}

void Cpu::IndirectY(void (Cpu::*opcode)(), bool rmw, bool write) {
    _val = _readPcAndInc();
    _addr16.l = _readZp(_val++);
    _addr16.h = _readZp(_val++);
    _paged = _system->mem->PageIfRequired(_addr16.w, _addr16.w + x);
    if (_paged) _addClocks();
    if (!rmw && !write) _testInterrupt();
    if (!write) _val = _read(_addr16.w + y);
    if (rmw) {
        if (!write) _testInterrupt();
        _write(_addr16.w + y, _val);
    }
    (this->*opcode)();
    if (write) {
        _testInterrupt();
        _write(_addr16.w + y, _val);
        _addClocks();
        _addClocks();
    }
}

void Cpu::Absolute(void (Cpu::*opcode)(), bool rmw, bool write) {
    _addr16.l = _readPcAndInc();
    _addr16.h = _readPcAndInc();
    if (!rmw && !write) _testInterrupt();
    if (!write) _val = _read(_addr16.w);
    if (rmw){
        if (!write) _testInterrupt();
        _write(_addr16.w, _val);
        _addClocks();
    }
    (this->*opcode)();
    if (write) {
        _testInterrupt();
        _write(_addr16.w, _val);
        _addClocks();
    }
}

void Cpu::AbsoluteX(void (Cpu::*opcode)(), bool rmw, bool write) {
    _addr16.l = _readPcAndInc();
    _addr16.h = _readPcAndInc();
    if (!rmw && !write) _testInterrupt();
    if (!write) _val = _read(_addr16.w + x);
    if (rmw) {
        if (!write) _testInterrupt();
        _write(_addr16.w + x, _val);
        _addClocks();
    }
    (this->*opcode)();
    if (write) {
        _testInterrupt();
        _write(_addr16.w + x, _val);
        _addClocks();
        _addClocks();
    }
}

void Cpu::AbsoluteY(void (Cpu::*opcode)(), bool rmw, bool write) {
    _addr16.l = _readPcAndInc();
    _addr16.h = _readPcAndInc();
    if (!rmw && !write) _testInterrupt();
    if (!write) _val = _read(_addr16.w + y);
    if (rmw) {
        if (!write) _testInterrupt();
        _write(_addr16.w + y, _val);
    }
    (this->*opcode)();
    if (write) {
        _testInterrupt();
        _write(_addr16.w + y, _val);
        _addClocks();
        _addClocks();
    }
}

void Cpu::Read(void (Cpu::*operation)(void (Cpu::*)(), bool, bool), void (Cpu::*opcode)()) {
    (this->*operation)(opcode, false, false);
}

void Cpu::Store(void (Cpu::*operation)(void (Cpu::*)(), bool, bool), void (Cpu::*opcode)()) {
    (this->*operation)(opcode, false, true);
}

void Cpu::Rmw(void (Cpu::*operation)(void (Cpu::*)(), bool, bool), void (Cpu::*opcode)()) {
    (this->*operation)(opcode, true, true);
}

void Cpu::_testInterrupt(){
    _interruptPending = ((_irq | _apu) & ~p.i) | _nmiPending;
}

bool Cpu::Interrupt(){
    if (!_interruptPending) return false;
    _addClocks();
    _addClocks();
    _writeSp(pc >> 8);
    _writeSp(pc >> 0);
    _writeSp((p | 0x20) & 0xEF);
    uint16_t vector = 0xfffe;  //IRQ
    if(_nmiPending) {
        _nmiPending = false;
        vector = 0xfffa;
    }
    _addr16.l = _read(vector++);
    p.i = 1;
    p.d = 0;
    _testInterrupt();
    _addr16.h = _read(vector++);
    _system->debug->trace.push_back(1);
    //_system->logger->Log("<<interrupt>>:  " + std::to_string(vector));
    pc = _addr16.w;
    return true;
}

void Cpu::Apu(bool line) {
    _apu = line;
}

void Cpu::Irq(bool line) {
    _irq = line;
}

void Cpu::Nmi(bool line) {
    if(!_nmi && line)
        _nmiPending = true;
    _nmi = line;
}