//
// Created by James Barker on 06/10/2018.
//

#include "Filter.h"

float FirstOrderFilter::Step(float x) {
    float y = (B0*x) + (B1*prevX) - (A1*prevY);
    prevY = y;
    prevX = x;
    return y;
}

FirstOrderFilter::FirstOrderFilter() {}

Filter::Filter() {}

LowPassFilter::LowPassFilter(float sampleRate, float cutOffFreq) {
    float c = sampleRate / M_PI / cutOffFreq;
    float a0i = 1 / (1 + c);
    B0 = a0i;
    B1 = a0i;
    A1 = (1 - c) * a0i;
}

HighPassFilter::HighPassFilter(float sampleRate, float cutOffFreq) {
    float c = sampleRate / M_PI / cutOffFreq;
    float a0i = 1 / (1 + c);
    B0 = c * a0i;
    B1 = -c * a0i;
    A1 = (1 - c) * a0i;
}
