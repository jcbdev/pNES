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
        case 0x69: Read(&Cpu::Immediate, &Cpu::Adc); break;
        case 0x65: Read(&Cpu::Zeropage, &Cpu::Adc); break;
        case 0x75: Read(&Cpu::ZeropageX, &Cpu::Adc); break;
        case 0x6D: Read(&Cpu::Absolute, &Cpu::Adc); break;
        case 0x7D: Read(&Cpu::AbsoluteX, &Cpu::Adc); break;
        case 0x79: Read(&Cpu::AbsoluteY, &Cpu::Adc); break;
        case 0x61: Read(&Cpu::IndirectX, &Cpu::Adc); break;
        case 0x71: Read(&Cpu::IndirectY, &Cpu::Adc); break;

        case 0x29: Read(&Cpu::Immediate, &Cpu::And); break;
        case 0x25: Read(&Cpu::Zeropage, &Cpu::And); break;
        case 0x35: Read(&Cpu::ZeropageX, &Cpu::And); break;
        case 0x2D: Read(&Cpu::Absolute, &Cpu::And); break;
        case 0x3D: Read(&Cpu::AbsoluteX, &Cpu::And); break;
        case 0x39: Read(&Cpu::AbsoluteY, &Cpu::And); break;
        case 0x21: Read(&Cpu::IndirectX, &Cpu::And); break;
        case 0x31: Read(&Cpu::IndirectY, &Cpu::And); break;

        case 0x0A: AslAccumulator(); break;

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
