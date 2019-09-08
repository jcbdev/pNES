//
// Created by jimbo on 24/09/18.
//

#include "Apu.h"
#include "Cpu.h"
#include "Memory.h"

IApu::IApu(ISystem *system) {
    _system = system;
}

Apu::Apu(ISystem *system) : IApu(system) {
    for (int i=0; i < 31; i++) {
        _pulseTable[i] = 95.52f / (8128.0f/(float)(i) + 100);
    }
    for (int i=0; i < 203; i++) {
        _tndTable[i] = 163.67f / (24329.0f/(float)(i) + 100);
    }

    //initialize channels
    _noise.shiftRegister = 1;
    _pulse1.channel = 1;
    _pulse2.channel = 2;
    //_dmc.system = system;
    cycle = 0;
}

void Apu::Step() {
    uint64_t cycle1 = cycle;
    cycle++;
    uint64_t cycle2 = cycle;
    _stepTimer();
    int32_t f1 = (int)((double)(cycle1) / _frameCounterRate);
    int32_t f2 = (int)((double)(cycle2) / _frameCounterRate);
    if (f1 != f2) _stepFrameCounter();
    int32_t s1 = (int)((double)(cycle1) / _sampleRate);
    int32_t s2 = (int)((double)(cycle2) / _sampleRate);
    if (s1 != s2) _sendSample();
}

void Apu::_sendSample() {
    float output = filter
}

float Apu::_output() {
    uint8_t p1 = _pulse1.output();
    uint8_t p2 = _pulse2.output();
    uint8_t t = _triangle.output();
    uint8_t n = _noise.output();
    uint8_t d = _dmc.output();
    float pulseOut = _pulseTable[p1+p2];
    float tndOut = _tndTable[(3*t) + (2*n) + d];
    return pulseOut + tndOut;
}

void Apu::_stepFrameCounter() {
    switch (_framePeriod){
        case 4:
            _frameValue = (_frameValue + 1) % 4;
            switch (_frameValue) {
                case 0: case 2:
                    _stepEnvelope();
                    break;
                case 1:
                    _stepEnvelope();
                    _stepSweep();
                    _stepLength();
                    break;
                case 3:
                    _stepEnvelope();
                    _stepSweep();
                    _stepLength();
                    _irq();
                    break;
            }
            break;
        case 5:
            _frameValue = (_frameValue + 1) % 5;
            switch (_frameValue) {
                case 0: case 2:
                    _stepEnvelope();
                    break;
                case 1: case 3:
                    _stepEnvelope();
                    _stepSweep();
                    _stepLength();
                    break;
            }
            break;
    }
}

void Apu::_stepTimer() {
    if ((cycle%2) == 0) {
        _pulse1.stepTimer();
        _pulse2.stepTimer();
        _noise.stepTimer();
        _dmc.stepTimer(_system);
    }
    _triangle.stepTimer();
}

void Apu::_stepEnvelope() {
    _pulse1.stepEnvelope();
    _pulse2.stepEnvelope();
    _triangle.stepCounter();
    _noise.stepEnvelope();
}

void Apu::_stepSweep() {
    _pulse1.stepSweep();
    _pulse2.stepSweep();
}

void Apu::_stepLength() {
    _pulse1.stepLength();
    _pulse2.stepLength();
    _triangle.stepLength();
    _noise.stepLength();
}

void Apu::_irq() {
    if (_frameIRQ) {
        _system->cpu->Irq(true);
    }
}

uint8_t Apu::ReadRegister(uint16_t addr) {
    switch (addr) {
        case 0x4015:
            return _readStatus();
            break;
        default:
            return 0;
    }
}

void Apu::WriteRegister(uint16_t addr, uint8_t data) {
    switch (addr) {
        case 0x4000:
            _pulse1.writeControl(data);
            break;
        case 0x4001:
            _pulse1.writeSweep(data);
            break;
        case 0x4002:
            _pulse1.writeTimerLow(data);
            break;
        case 0x4003:
            _pulse1.writeTimerHigh(data);
            break;
        case 0x4004:
            _pulse2.writeControl(data);
            break;
        case 0x4005:
            _pulse2.writeSweep(data);
            break;
        case 0x4006:
            _pulse2.writeTimerLow(data);
            break;
        case 0x4007:
            _pulse2.writeTimerHigh(data);
            break;
        case 0x4008:
            _triangle.writeControl(data);
            break;
        case 0x4009:
        case 0x4010:
            _dmc.writeControl(data);
            break;
        case 0x4011:
            _dmc.writeValue(data);
            break;
        case 0x4012:
            _dmc.writeAddress(data);
            break;
        case 0x4013:
            _dmc.writeLength(data);
            break;
        case 0x400A:
            _triangle.writeTimerLow(data);
            break;
        case 0x400B:
            _triangle.writeTimerHigh(data);
            break;
        case 0x400C:
            _noise.writeControl(data);
            break;
        case 0x400D:
        case 0x400E:
            _noise.writePeriod(data);
            break;
        case 0x400F:
            _noise.writeLength(data);
            break;
        case 0x4015:
            _writeControl(data);
            break;
        case 0x4017:
            _writeFrameCounter(data);
            break;
            // default:
            // 	log.Fatalf("unhandled apu register write at address: 0x%04X", address)
    }
}

uint8_t Apu::_readStatus() {
    uint8_t status = 0;
    if (_pulse1.lengthValue > 0) status |= 1;
    if (_pulse2.lengthValue > 0) status |= 2;
    if (_triangle.lengthValue > 0) status |= 4;
    if (_noise.lengthValue > 0) status |= 8;
    if (_dmc.currentLength > 0)
        return status;
}

void Apu::_writeControl(uint8_t value) {
    _pulse1.enabled = (value&1) == 1;
    _pulse2.enabled = (value&2) == 2;
    _triangle.enabled = (value&4) == 4;
    _noise.enabled = (value&8) == 8;
    _dmc.enabled = (value&16) == 16;
    if (!_pulse1.enabled) _pulse1.lengthValue = 0;
    if (!_pulse2.enabled) _pulse2.lengthValue = 0;
    if (!_triangle.enabled) _triangle.lengthValue = 0;
    if (!_noise.enabled) _noise.lengthValue = 0;
    if (!_dmc.enabled) _dmc.currentLength = 0;
    else if (_dmc.currentLength == 0) _dmc.restart();
}

void Apu::_writeFrameCounter(uint8_t value) {
    _framePeriod = 4 + ((value>>7)&1);
    _frameIRQ = ((value>>6)&1) == 0;
    // apu.frameValue = 0
    if (_framePeriod == 5) {
        _stepEnvelope();
        _stepSweep();
        _stepLength();
    }
}

//PULSE
void Pulse::writeControl(uint8_t value){
    dutyMode = ((value >> 6) & 3);
    lengthEnabled = ((value>>5)&1) == 0;
    envelopeLoop = ((value>>5)&1) == 1;
    envelopeEnabled = ((value>>4)&1) == 0;
    envelopePeriod = value & 15;
    constantVolume = value & 15;
    envelopeStart = true;
}

void Pulse::writeSweep(uint8_t value){
    sweepEnabled = ((value>>7)&1) == 1;
    sweepPeriod = ((value>>4)&7) + 1;
    sweepNegate = ((value>>3)&1) == 1;
    sweepShift = value & 7;
    sweepReload = true;
}

void Pulse::writeTimerLow(uint8_t value) {
    timerPeriod = (timerPeriod & 0xFF00) | (uint16_t)(value);
}

void Pulse::writeTimerHigh(uint8_t value) {
    lengthValue = LengthCounter[value>>3];
    timerPeriod = (timerPeriod & 0x00FF) | ((uint16_t)(value&7) << 8);
    envelopeStart = true;;
    dutyValue = 0;
}

void Pulse::stepTimer() {
    if (timerValue == 0) {
        timerValue = timerPeriod;
        dutyValue = (dutyValue + 1) % 8;
    } else {
        timerValue--;
    }
}

void Pulse::stepEnvelope() {
    if (envelopeStart) {
        envelopeVolume = 15;
        envelopeValue = envelopePeriod;
        envelopeStart = false;
    } else if (envelopeValue > 0) {
        envelopeValue--;
    } else {
        if (envelopeVolume > 0) {
            envelopeVolume--;
        } else if (envelopeLoop) {
            envelopeVolume = 15;
        }
        envelopeValue = envelopePeriod;
    }
}

void Pulse::stepSweep() {
    if (sweepReload) {
        if (sweepEnabled && sweepValue == 0) {
            sweep();
        }
        sweepValue = sweepPeriod;
        sweepReload = false;
    } else if (sweepValue > 0) {
        sweepValue--;
    } else {
        if (sweepEnabled) {
            sweep();
        }
        sweepValue = sweepPeriod;
    }
}

void Pulse::stepLength() {
    if (lengthEnabled && lengthValue > 0) {
        lengthValue--;
    }
}

void Pulse::sweep() {
    auto delta = timerPeriod >> sweepShift;
    if (sweepNegate) {
        timerPeriod -= delta;
        if (channel == 1) {
            timerPeriod--;
        }
    } else {
        timerPeriod += delta;
    }
}

uint8_t Pulse::output() {
    if (!enabled)
    {
        return 0;
    }
    if (lengthValue == 0)
    {
        return 0;
    }
    if (DutyTable[dutyMode][dutyValue] == 0)
    {
        return 0;
    }
    if (timerPeriod < 8 || timerPeriod > 0x7FF)
    {
        return 0;
    }
// if !p.sweepNegate && p.timerPeriod+(p.timerPeriod>>p.sweepShift) > 0x7FF {
// 	return 0
// }
    if (envelopeEnabled)
    {
        return envelopeVolume;
    } else {
        return constantVolume;
    }
}


//TRIANGLE

void Triangle::writeControl(uint8_t value) {
    lengthEnabled = ((value>>7)&1) == 0;
    counterPeriod = value & 0x7F;
}

void Triangle::writeTimerLow(uint8_t value) {
    timerPeriod = (timerPeriod & 0xFF00) | (uint16_t)(value);
}

void Triangle::writeTimerHigh(uint8_t value) {
    lengthValue = LengthCounter[value>>3];
    timerPeriod = (timerPeriod & 0x00FF) | ((uint16_t)(value&7) << 8);
    timerValue = timerPeriod;
    counterReload = true;
}

void Triangle::stepTimer() {
    if (timerValue == 0) {
        timerValue = timerPeriod;
        if (lengthValue > 0 && counterValue > 0) {
            dutyValue = (dutyValue + 1) % 32;
        }
    } else {
        timerValue--;
    }
}

void Triangle::stepLength() {
    if (lengthEnabled && lengthValue > 0) {
        lengthValue--;
    }
}

void Triangle::stepCounter() {
    if (counterReload) {
        counterValue = counterPeriod;
    } else if (counterValue > 0) {
        counterValue--;
    }
    if (lengthEnabled) {
        counterReload = false;
    }
}

uint8_t Triangle::output() {
    if (!enabled)
    {
        return 0;
    }
    if (lengthValue == 0)
    {
        return 0;
    }
    if (counterValue == 0)
    {
        return 0;
    }
    return TriangleTable[dutyValue];
}

//NOOOOIIIIISSSEEEE

void Noise::writeControl(uint8_t byte) {
    lengthEnabled = ((byte >> 5) & 1) == 0;
    envelopeLoop = ((byte >> 5) & 1) == 1;
    envelopeEnabled = ((byte >> 4) & 1) == 0;
    envelopePeriod = byte & 15;
    constantVolume = byte & 15;
    envelopeStart = true;
}

void Noise::writePeriod(uint8_t value) {
    mode = (value & 0x80) == 0x80;
    timerPeriod = NtscNoisePeriod[value & 0x0F];
}

void Noise::writeLength(uint8_t value) {
    lengthValue = LengthCounter[value >> 3];
    envelopeStart = true;
}

void Noise::stepTimer() {
    if (timerValue == 0) {
        timerValue = timerPeriod;
        uint8_t shift = 0;
        if (mode) {
            shift = 6;
        } else {
            shift = 1;
        }
        auto b1 = shiftRegister & 1;
        auto b2 = (shiftRegister >> shift) & 1;
        shiftRegister >>= 1;
        shiftRegister |= (b1 ^ b2) << 14;
    } else {
        timerValue--;
    }
}

void Noise::stepEnvelope() {
    if (envelopeStart) {
        envelopeVolume = 15;
        envelopeValue = envelopePeriod;
        envelopeStart = false;
    } else if (envelopeValue > 0) {
        envelopeValue--;
    } else {
        if (envelopeVolume > 0) {
            envelopeVolume--;
        } else if (envelopeLoop) {
            envelopeVolume = 15;
        }
        envelopeValue = envelopePeriod;
    }
}

void Noise::stepLength() {
    if (lengthEnabled && lengthValue > 0) {
        lengthValue--;
    }
}

uint8_t Noise::output() {
    if (!enabled)
    {
        return 0;
    }
    if (lengthValue == 0)
    {
        return 0;
    }
    if ((shiftRegister & 1) == 1)
    {
        return 0;
    }
    if (envelopeEnabled)
    {
        return envelopeVolume;
    } else {
        return constantVolume;
    }
}

//DMC

void DMC::writeControl(uint8_t value) {
    irq = (value & 0x80) == 0x80;
    loop = (value & 0x40) == 0x40;
    tickPeriod = NtscDmcPeriod[value & 0x0F];
}

void DMC::writeValue(uint8_t value) {
    value = value & 0x7F;
}

void DMC::writeAddress(uint8_t value) {
// Sample address = %11AAAAAA.AA000000
    sampleAddress = 0xC000 | ((uint16_t)(value) << 6);
}

void DMC::writeLength(uint8_t value) {
// Sample length = %0000LLLL.LLLL0001
    sampleLength = ((uint16_t)(value) << 4) | 1;
}

void DMC::restart() {
    currentAddress = 0;
    currentLength = 0;
}

void DMC::stepTimer(ISystem* system) {
    if (!enabled) {
        return;
    }
    stepReader(system);
    if (tickValue == 0) {
        tickValue = tickPeriod;
        stepShifter();
    } else {
        tickValue--;
    }
}

void DMC::stepReader(ISystem* system) {
    if (currentLength > 0 && bitCount == 0) {
        system->cpu->clocks += 4;
        shiftRegister = system->mem->Read(currentAddress);
        bitCount = 8;
        currentAddress++;
        if (currentAddress == 0) {
            currentAddress = 0x8000;
        }
        currentLength--;
        if (currentLength == 0 && loop) {
            this->restart();
        }
    }
}

void DMC::stepShifter() {
    if (bitCount == 0) {
        return;
    }
    if ((shiftRegister&1) == 1) {
        if (value <= 125) {
            value += 2;
        }
    } else {
        if (value >= 2) {
            value -= 2;
        }
    }
    shiftRegister >>= 1;
    bitCount--;
}

uint8_t DMC::output() {
    return value;
}