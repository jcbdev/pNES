//
// Created by James Barker on 27/08/2018.
//

#include "NoDebug.h"

NoDebug::NoDebug(ISystem* system) : IDebug(system)
{
    enabled = false;
    pause = false;
    step = false;
}

std::string NoDebug::Decode(int pc, int* increment, bool dynamic)
{
    return "";
}

void NoDebug::Refresh()
{

}

void NoDebug::GoNext()
{

}

void NoDebug::GoPrev()
{

}

void NoDebug::AddBrk()
{

}

bool NoDebug::isBrk(int line)
{
    return false;
}

void NoDebug::Break()
{

}