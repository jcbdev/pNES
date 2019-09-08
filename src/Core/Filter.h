//
// Created by James Barker on 06/10/2018.
//

#ifndef PNES_FILTER_H
#define PNES_FILTER_H

#include <stdint.h>

class IFilter {
public:
    virtual float Step(float x) = 0;

};


struct Filter {
    enum int32_t { HiPassStrong = 225574, HiPassWeak = 57593, LoPass = 86322413 };

    int64_t hipass_strong;
    int64_t hipass_weak;
    int64_t lopass;

    int64_t run_hipass_strong(int32_t sample);
    int64_t run_hipass_weak(int32_t sample);
    int64_t run_lopass(int32_t sample);
} filter;

#endif //PNES_FILTER_H
