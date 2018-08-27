//
// Created by James Barker on 27/08/2018.
//

#ifndef PNES_NODEBUG_H
#define PNES_NODEBUG_H

#include "System.h"
#include "Debug.h"

class NoDebug : public IDebug {
public:
    explicit NoDebug(ISystem* system);

    std::string Decode(int pc, int* increment, bool dynamic) override;
    void Refresh() override;
    void GoNext() override;
    void GoPrev() override;
    void AddBrk() override;
    bool isBrk(int line) override;
    void Break() override;
};


#endif //PNES_NODEBUG_H
