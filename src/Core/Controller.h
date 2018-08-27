//
// Created by James Barker on 27/08/2018.
//

#ifndef PNES_CONTROLLER_H
#define PNES_CONTROLLER_H


#include <cstdint>
#include "System.h"

class IController {
public:
    explicit IController(ISystem* system);
    bool buttons[8];
    uint8_t index;
    uint8_t strobe;

    virtual uint8_t Read() = 0;
    virtual void Write(uint8_t value) = 0;

    virtual void SetButton(uint8_t button) = 0;
    virtual void UnsetButton(uint8_t button) = 0;
protected:
    ISystem* _system;
};


class Controller : public IController {
public:
    explicit Controller(ISystem* system);

    uint8_t Read() override;
    void Write(uint8_t value) override;

    void SetButton(uint8_t button) override;
    void UnsetButton(uint8_t button) override;
};

#endif //PNES_CONTROLLER_H
