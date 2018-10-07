//
// Created by James Barker on 06/10/2018.
//

#include "Filter.h"

int64_t Filter::run_hipass_strong(int32_t sample) {
    hipass_strong += ((((int64_t)sample << 16) - (hipass_strong >> 16)) * HiPassStrong) >> 16;
    return sample - (hipass_strong >> 32);
}

int64_t Filter::run_hipass_weak(int32_t sample) {
    hipass_weak += ((((int64_t)sample << 16) - (hipass_weak >> 16)) * HiPassWeak) >> 16;
    return sample - (hipass_weak >> 32);
}

int64_t Filter::run_lopass(int32_t sample) {
    lopass += ((((int64_t)sample << 16) - (lopass >> 16)) * LoPass) >> 16;
    return (lopass >> 32);
}