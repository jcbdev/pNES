//
// Created by James Barker on 27/08/2018.
//

#include "Controller.h"

IController::IController(ISystem *system) {
    _system = system;
}

Controller::Controller(ISystem *system) : IController(system)
{
    for (int i = 0; i < 8; i++){
        buttons[i] = false;
    }
}

uint8_t Controller::Read() {
    uint8_t value = 0;
    if (index < 8 && buttons[index]){
        value = 1;
    }
    index++;
    if ((strobe&1) == 1) {
        index = 0;
    }
    return value;
}

void Controller::Write(uint8_t value) {
    strobe = value;
    if ((strobe&1) == 1) {
        index = 0;
    }
}

void Controller::SetButton(uint8_t button) {
    buttons[button] = true;
}

void Controller::UnsetButton(uint8_t button) {
    buttons[button] = false;
}
