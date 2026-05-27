#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include "Common.h"

class InputHandler {
public:
    InputPacket scan(); // deve ler os sensores e retornar um pacote
};
#endif