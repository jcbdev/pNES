//
// Created by jimbo on 24/09/18.
//

#ifndef PNES_APU_H
#define PNES_APU_H


#include "System.h"

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
};

class Filter {
public:
    float Step(float x);

};

class IApu {
public:
    explicit IApu(ISystem* system);
    virtual uint8_t Read(uint16_t addr) = 0;
    virtual void Write(uint16_t addr, uint8_t data) = 0;

    int32_t clocks;

protected:
    ISystem* _system;

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
};

class Apu : IApu {
public:
    explicit Apu(ISystem *system);
    uint8_t Read(uint16_t addr) override;
    void Write(uint16_t addr, uint8_t data) override;

private:
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