#pragma once
#include <citro2d.h>
#include "Common.h"


class Cena {
public:
    virtual void setStatus(std::string msg) {}
    virtual std::string getIP() { return ""; }
    virtual ~Cena() {} // destrutor
    virtual int update(const InputPacket& packet) = 0; // "= 0" significa que � obrigat�rio nas filhas
    virtual void draw(C3D_RenderTarget* top, C3D_RenderTarget* bottom) = 0;
};