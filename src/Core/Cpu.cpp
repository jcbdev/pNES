//
// Created by James Barker on 16/11/2016.
//

#include "Cpu.h"

ICpu::ICpu(std::shared_ptr<Memory> &memory) {
    x = 0;
    y = 0;
    a = 0;
    p = 0x34;
    s = 0xFD;
    pc = 0x8000;

    _mem = memory;
    error = false;
}

Cpu::Cpu(std::shared_ptr<Memory>& memory) : ICpu::ICpu(memory) {

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
    pc = 0x8000;

    _mem->Reset();
    error = false;
}

void Cpu::Cycle() {

    switch(_readPcAndInc()){
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

        //Ldx
        case 0xB6: Read(&Cpu::ZeropageY, &Cpu::Ldx); break;

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
    _readPc();
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
    _val = _readPcAndInc();
    if (condition) {
        uint16_t val16 = pc + (uint8_t)_val;
        _mem->PageIfRequired(pc, val16);
        _readPc();
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

void Cpu::Eor(){
    a ^= _val;
    p.n = (a & 0x80);
    p.z = (a == 0);
}

void Cpu::FlagClear(bool &flag){
    _readPc();
    flag = false;
}

void Cpu::FlagSet(bool &flag){
    _readPc();
    flag = true;
}

void Cpu::Ldx() {
    x = _val;
    p.n = x & 0x80;
    p.z = x == 0;
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
    if (write) _mem->Write(_addr16.w + x, _val);
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