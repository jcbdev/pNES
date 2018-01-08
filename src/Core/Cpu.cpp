//
// Created by James Barker on 16/11/2016.
//

#include "Cpu.h"

ICpu::ICpu(IMemory* memory, ILogger* logger) {
    x = 0;
    y = 0;
    a = 0;
    p = 0x34;
    s = 0xFD;
    _mem = memory;
    Address16 pcaddr;
    pcaddr.l = _mem->Read(0xFFFC);
    pcaddr.h = _mem->Read(0xFFFD);
    pc = pcaddr.w;
//    pc = 0x8000;

    error = false;
    _logger = logger;
    ticks = 0;
}

Cpu::Cpu(IMemory* memory, ILogger* logger) : ICpu::ICpu(memory, logger) {
    _interrupt_pending = false;
    _nmi = false;
    _nmi_pending = false;
    _irq = false;
    _apu = false;
}

uint8_t Cpu::_readPcAndInc() {
    return _mem->Read(pc++);
}

uint8_t Cpu::_readPc() {
    return _mem->Read(pc);
}

void Cpu::Reset() {
    x = 0;
    y = 0;
    a = 0;
    p = 0x34;
    s = 0xFD;
    Address16 pcaddr;
    pcaddr.l = _mem->Read(0xFFFC);
    pcaddr.h = _mem->Read(0xFFFD);
    pc = pcaddr.w;
//    pc = 0x8000;

    _nmi = false;
    _nmi_pending = false;
    _irq = false;
    _apu = false;

    _mem->Reset();
    error = false;
}

void Cpu::Cycle() {
    uint8_t opcode = _readPcAndInc();
    _logger->Log(std::to_string(opcode));
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
    p.c = _val & 0x80;
    _val <<= 1;
    p.n = (_val & 0x80);
    p.z = (_val == 0);
}

void Cpu::AslAccumulator(){
    //_readPc();
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
    _val = _readPcAndInc();
    if (condition) {
        uint16_t val16 = pc + (uint8_t)_val;
        _mem->PageIfRequired(pc, val16);
        _testInterrupt();
        //_readPc();
        pc = val16;
    }
}

void Cpu::Brk(){
    _readPcAndInc();
    _writeSp(pc >> 8);
    _writeSp(pc >> 0);
    _writeSp(p | 0x30);
    _addr16.l = _mem->Read(0xfffe);
    p.i = 1;
    p.d = 0;
    _addr16.h = _mem->Read(0xffff);
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
    _val--;
    p.n = (_val & 0x80);
    p.z = (_val == 0);
}

void Cpu::Dex(){
    //_readPc();
    x--;
    p.n = (x & 0x80);
    p.z = (x == 0);
};

void Cpu::Dey(){
    //_readPc();
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
    //_readPc();
    flag = false;
}

void Cpu::FlagSet(bool &flag){
    //_readPc();
    flag = true;
}

void Cpu::Inc(){
    _val++;
    p.n = (_val & 0x80);
    p.z = (_val == 0);
}

void Cpu::Inx(){
    //_readPc();
    x++;
    p.n = (x & 0x80);
    p.z = (x == 0);
}

void Cpu::Iny(){
    //_readPc();
    y++;
    p.n = (y & 0x80);
    p.z = (y == 0);
}

void Cpu::JmpAbsolute(){
    _addr16.l = _readPcAndInc();
    _addr16.h = _readPcAndInc();
    pc = _addr16.w;
}

void Cpu::JmpIndirect() {
    _addr16.l = _readPcAndInc();
    _addr16.h = _readPcAndInc();
    Address16 indirectAddr;
    indirectAddr.l = _mem->Read(_addr16.w); _addr16.l++;
    indirectAddr.h = _mem->Read(_addr16.w); _addr16.l++;
    pc = indirectAddr.w;
}

void Cpu::Jsr(){
    _addr16.l = _readPcAndInc();
    _addr16.h = _readPcAndInc();
    //_readPc();
    pc--;
    _writeSp((uint8_t)(pc >> 8));
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
}

void Cpu::LsrAccumulator() {
    //_readPc();
    p.c = a & 0x01;
    a >>= 1;
    p.n = (a & 0x80);
    p.z = (a == 0);
}

void Cpu::Nop(){
    //_readPc();
}

void Cpu::Ora(){
    a |= _val;
    p.n = (a & 0x80);
    p.z = (a == 0);
}

void Cpu::Pha(){
    //_readPc();
    _writeSp(a);
}

void Cpu::Php(){
    //_readPc();
    _writeSp(p | 0x30);
}

void Cpu::Pla(){
    //_readPc();
    //_readPc();
    a = _readSp();
    p.n = (a & 0x80);
    p.z = (a == 0);
}

void Cpu::Plp(){
    //_readPc();
    //_readPc();
    p = _readSp();
}

void Cpu::RolAccumulator(){
    uint8_t carry = (uint8_t)p.c;
    p.c = a & 0x80;
    a = (a << 1) | carry;
    p.n = (a & 0x80);
    p.z = (a == 0);
}

void Cpu::Rol() {
    uint8_t carry = (uint8_t)p.c;
    p.c = _val & 0x80;
    _val = (_val << 1) | carry;
    p.n = (_val & 0x80);
    p.z = (_val == 0);
}

void Cpu::RorAccumulator() {
    uint8_t carry = (uint8_t)p.c << 7;
    p.c = a & 0x01;
    a = carry | (a >> 1);
    p.n = (a & 0x80);
    p.z = (a == 0);
}

void Cpu::Ror() {
    uint8_t carry = (uint8_t)p.c << 7;
    p.c = _val & 0x01;
    _val = carry | (_val >> 1);
    p.n = (_val & 0x80);
    p.z = (_val == 0);
}

void Cpu::Rti() {
    //_readPc();
    //_readPc();
    p = _readSp();
    _addr16.l = _readSp();
    _addr16.h = _readSp();
    pc = _addr16.w;
}

void Cpu::Rts() {
    //_readPc();
    //_readPc();
    _addr16.l = _readSp();
    _addr16.h = _readSp();
    //_readPc();
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
    //_readPc();
    x = a;
    p.n = (x & 0x80);
    p.z = (x == 0);
}

void Cpu::Txa() {
    //_readPc();
    a = x;
    p.n = (a & 0x80);
    p.z = (a == 0);
}

void Cpu::Tay() {
    //_readPc();
    y = a;
    p.n = (y & 0x80);
    p.z = (y == 0);
}

void Cpu::Tya() {
    //_readPc();
    a = y;
    p.n = (a & 0x80);
    p.z = (a == 0);
}

void Cpu::Txs() {
    //_readPc();
    s = x;
}

void Cpu::Tsx() {
    //_readPc();
    x = s;
    p.n = (x & 0x80);
    p.z = (x == 0);
}

void Cpu::IllegalNopZeroPage(){
    _val = _readPcAndInc();
    _mem->ReadZP(_val);
}

void Cpu::IllegalNopAbsolute(){
    _addr16.l = _readPcAndInc();
    _addr16.h = _readPcAndInc();
    //_readPc();
}

void Cpu::IllegalNopZeroPageX(){
    _val = _readPcAndInc();
    _mem->ReadZP(_val);
    _mem->ReadZP(_val + x);
}

void Cpu::IllegalNopImplied(){
    //_readPc();
}

void Cpu::IllegalNopAbsoluteX(){
    _addr16.l = _readPcAndInc();
    _addr16.h = _readPcAndInc();
    _mem->PageIfRequired(_addr16.w, _addr16.w + x);
    //_readPc();
}

void Cpu::IllegalArrImmediate() {
    _val = _readPcAndInc();
    a &= _val;
    a = (p.c << 7) | (a >> 1);
    p.n = (a & 0x80);
    p.z = (a == 0);
    p.c = (a & 0x40);
    p.v = p.c ^ ((a >> 5) & 1);
}

void Cpu::IllegalNopImmediate() {
    //_readPc();
}

void Cpu::Immediate(void (Cpu::*opcode)(), bool rmw, bool write) {
    _val = _readPcAndInc();
    (this->*opcode)();
}

void Cpu::Zeropage(void (Cpu::*opcode)(), bool rmw, bool write) {
    uint8_t zp = _readPcAndInc();
    _val = _mem->ReadZP(zp);
    if (rmw) _mem->WriteZP(zp, _val);
    (this->*opcode)();
    if (write) _mem->WriteZP(zp, _val);
}

void Cpu::ZeropageX(void (Cpu::*opcode)(), bool rmw, bool write) {
    uint8_t zp = _readPcAndInc();
    _val = _mem->ReadZP(zp + x);
    if (rmw) _mem->WriteZP(zp + x, _val);
    (this->*opcode)();
    if (write) _mem->WriteZP(zp + x, _val);
}

void Cpu::ZeropageY(void (Cpu::*opcode)(), bool rmw, bool write) {
    uint8_t zp = _readPcAndInc();
    _val = _mem->ReadZP(zp + y);
    if (rmw) _mem->WriteZP(zp + y, _val);
    (this->*opcode)();
    if (write) _mem->WriteZP(zp + y, _val);
}

void Cpu::IndirectX(void (Cpu::*opcode)(), bool rmw, bool write) {
    uint8_t zp = _readPcAndInc();
    _mem->ReadZP(zp);
    _addr16.l = _mem->ReadZP(zp++ + x);
    _addr16.h = _mem->ReadZP(zp++ + x);
    _val = _mem->Read(_addr16.w);
    if (rmw) _mem->Write(_addr16.w, _val);
    (this->*opcode)();
    if (write) _mem->Write(_addr16.w, _val);
}

void Cpu::IndirectY(void (Cpu::*opcode)(), bool rmw, bool write) {
    _val = _readPcAndInc();
    _addr16.l = _mem->ReadZP(_val++);
    _addr16.h = _mem->ReadZP(_val++);
    _mem->PageIfRequired(_addr16.w, _addr16.w + y);
    _val = _mem->Read(_addr16.w + y);
    if (rmw) _mem->Write(_addr16.w + y, _val);
    (this->*opcode)();
    if (write) _mem->Write(_addr16.w + y, _val);
}

void Cpu::Absolute(void (Cpu::*opcode)(), bool rmw, bool write) {
    _addr16.l = _readPcAndInc();
    _addr16.h = _readPcAndInc();
    _val = _mem->Read(_addr16.w);
    if (rmw) _mem->Write(_addr16.w, _val);
    (this->*opcode)();
    if (write) _mem->Write(_addr16.w, _val);
}

void Cpu::AbsoluteX(void (Cpu::*opcode)(), bool rmw, bool write) {
    _addr16.l = _readPcAndInc();
    _addr16.h = _readPcAndInc();
    _val = _mem->Read(_addr16.w + x);
    if (rmw) _mem->Write(_addr16.w + x, _val);
    (this->*opcode)();
    if (write) _mem->Write(_addr16.w + x, _val);
}

void Cpu::AbsoluteY(void (Cpu::*opcode)(), bool rmw, bool write) {
    _addr16.l = _readPcAndInc();
    _addr16.h = _readPcAndInc();
    _val = _mem->Read(_addr16.w + y);
    if (rmw) _mem->Write(_addr16.w + y, _val);
    (this->*opcode)();
    if (write) _mem->Write(_addr16.w + y, _val);
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

void Cpu::_writeSp(uint8_t data){
    _mem->Write(0x0100 | s--, data);
}

uint8_t Cpu::_readSp(){
    return _mem->Read(0x0100 | ++s);
}

void Cpu::_testInterrupt(){
    _interrupt_pending = ((_irq | _apu) & ~p.i) | _nmi_pending;
}

bool Cpu::Interrupt(){
    //_readPc();
    //_readPc();
    if (!_interrupt_pending) return false;
    _writeSp(pc >> 8);
    _writeSp(pc >> 0);
    _writeSp(p | 0x20);
    uint16_t vector = 0xfffe;  //IRQ
    if(_nmi) {
        _nmi = false;
        vector = 0xfffa;
    }
    _addr16.l = _mem->Read(vector++);
    p.i = 1;
    p.d = 0;
    _testInterrupt();
    _addr16.h = _mem->Read(vector++);
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
    if(!_nmi && line) _nmi_pending = true;
    _nmi = line;
}