//
// Created by James Barker on 06/10/2018.
//

#ifndef PNES_FILTER_H
#define PNES_FILTER_H

#include <stdint.h>
#include <math.h>

class Filter {
protected:
    Filter(); //Prevent instantiation

public:
    virtual float Step(float x) = 0;
};

class FirstOrderFilter : Filter {
public:
    float Step(float x) override;

protected:
    FirstOrderFilter();
    float B0;
    float B1;
    float A1;
    float prevX;
    float prevY;
};

class LowPassFilter : FirstOrderFilter {
public:
    LowPassFilter(float sampleRate, float cutOffFreq);
};

class HighPassFilter : FirstOrderFilter {
public:
    HighPassFilter(float sampleRate, float cutOffFreq);
};

#endif //PNES_FILTER_H
