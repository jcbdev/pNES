//
// Created by jimbo on 07/04/19.
//

#include "ControllerStub.h"

uint8_t ControllerStub::Read() {
    return 0;
}

void ControllerStub::Write(uint8_t value) {

}

void ControllerStub::SetButton(uint8_t button) {

}

void ControllerStub::UnsetButton(uint8_t button) {

}

ControllerStub::ControllerStub(ISystem *system) : IController(system) {

}
