//
// Created by jimbo on 24/09/18.
//

#ifndef PNES_APU_H
#define PNES_APU_H


#include "System.h"

const uint8_t LengthCounter[32] = {
        0x0a, 0xfe, 0x14, 0x02, 0x28, 0x04, 0x50, 0x06, 0xa0, 0x08, 0x3c, 0x0a, 0x0e, 0x0c, 0x1a, 0x0e,
        0x0c, 0x10, 0x18, 0x12, 0x30, 0x14, 0x60, 0x16, 0xc0, 0x18, 0x48, 0x1a, 0x10, 0x1c, 0x20, 0x1e,
};

const uint16_t NtscNoisePeriod[16] = {
        4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068,
};


const uint16_t NtscDmcPeriod[16] = {
        428, 380, 340, 320, 286, 254, 226, 214, 190, 160, 142, 128, 106, 84, 72, 54,
};

const uint16_t PalDmcPeriod[16] = {
        398, 354, 316, 298, 276, 236, 210, 198, 176, 148, 132, 118,  98, 78, 66, 50,
};

const uint8_t DutyTable[4][8] = {
        {0, 1, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1, 0, 0, 0},
        {1, 0, 0, 1, 1, 1, 1, 1},
};

const uint8_t TriangleTable[32] {
        15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
};

struct Pulse {
    bool enabled;
    uint8_t channel;
    bool lengthEnabled;
    uint8_t lengthValue;
    uint16_t timerPeriod;
    uint16_t timerValue;
    uint8_t dutyMode;
    uint8_t dutyValue;
    bool sweepReload;
    bool sweepEnabled;
    bool sweepNegate;
    uint8_t sweepShift;
    uint8_t sweepPeriod;
    uint8_t sweepValue;
    bool envelopeEnabled;
    bool envelopeLoop;
    bool envelopeStart;
    uint8_t envelopePeriod;
    uint8_t envelopeValue;
    uint8_t envelopeVolume;
    uint8_t constantVolume;

    void writeControl(uint8_t value);

    void writeSweep(uint8_t value);

    void writeTimerLow(uint8_t value);

    void writeTimerHigh(uint8_t value);

    void stepTimer();

    void stepEnvelope();

    void stepSweep();

    void stepLength();

    void sweep();

    uint8_t output();
};

struct Triangle {
    bool enabled;
    bool lengthEnabled;
    uint8_t lengthValue;
    uint16_t timerPeriod;
    uint16_t timerValue;
    uint8_t dutyValue;
    uint8_t counterPeriod;
    uint8_t counterValue;
    bool counterReload;

    void writeControl(uint8_t value);

    void writeTimerLow(uint8_t value);

    void writeTimerHigh(uint8_t value);

    void stepTimer();

    void stepLength();

    void stepCounter();

    uint8_t output();
};

struct Noise {
    bool enabled;
    bool mode;
    uint16_t shiftRegister;
    bool lengthEnabled;
    uint8_t lengthValue;
    uint16_t timerPeriod;
    uint16_t timerValue;
    bool envelopeEnabled;
    bool envelopeLoop;
    bool envelopeStart;
    uint8_t envelopePeriod;
    uint8_t envelopeValue;
    uint8_t envelopeVolume;
    uint8_t constantVolume;

    void writePeriod(uint8_t value);

    void writeLength(uint8_t value);

    void writeControl(uint8_t byte);

    void stepTimer();

    void stepEnvelope();

    void stepLength();

    uint8_t output();
};

struct DMC {
    bool enabled;
    uint8_t value;
    uint16_t sampleAddress;
    uint16_t sampleLength;
    uint16_t currentAddress;
    uint16_t currentLength;
    uint8_t shiftRegister;
    uint8_t bitCount;
    uint8_t tickPeriod;
    uint8_t tickValue;
    bool loop;
    bool irq;
    void restart();

    void writeControl(uint8_t value);

    void writeValue(uint8_t value);

    void writeAddress(uint8_t value);

    void writeLength(uint8_t value);

    void stepTimer();

    void stepReader();

    void stepShifter();

    uint8_t output();
};

class IApu {
public:
    explicit IApu(ISystem* system);
    virtual uint8_t ReadRegister(uint16_t addr) = 0;
    virtual void WriteRegister(uint16_t addr, uint8_t data) = 0;

    int32_t clocks;

protected:
    ISystem* _system;
};

class Apu : IApu {
public:
    explicit Apu(ISystem *system);
    uint8_t ReadRegister(uint16_t addr) override;
    void WriteRegister(uint16_t addr, uint8_t data) override;

private:
    uint8_t _readStatus();
    void _writeControl(uint8_t value);

    float _pulseTable[32];
    float _tndTable[2013];
    double _sampleRate;
    float _channel;

    Pulse _pulse1;
    Pulse _pulse2;
    Triangle _triangle;
    Noise _noise;
    DMC _dmc;
    uint8_t _framePeriod;
    uint8_t _frameValue;
    bool _frameIRQ;

};




#endif //PNES_APU_H
