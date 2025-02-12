#pragma once

#include "iDDS.hpp"

class IDDSStateMachine
{
private:
    OperationalStatus currentState;
    IDDSStateMachine() : currentState(OperationalStatus::OpStatusDefault) {}

public:
    static IDDSStateMachine &getInstance(){ static IDDSStateMachine instance; return instance; }

    IDDSStateMachine(const IDDSStateMachine &) = delete;
    IDDSStateMachine &operator=(const IDDSStateMachine &) = delete;

    void setState(OperationalStatus state) { currentState = state; }
    OperationalStatus getState() { return currentState; }
};