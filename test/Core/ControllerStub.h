//
// Created by jimbo on 07/04/19.
//

#ifndef PNES_CONTROLLERSTUB_H
#define PNES_CONTROLLERSTUB_H


#include "../../src/Core/Controller.h"

class ControllerStub : public IController {
public:
    explicit ControllerStub(ISystem* system);
    bool buttons[8];
    uint8_t index;
    uint8_t strobe;

    uint8_t Read() override;
    void Write(uint8_t value) override;

    void SetButton(uint8_t button) override;
    void UnsetButton(uint8_t button) override;

};


#endif //PNES_CONTROLLERSTUB_H
