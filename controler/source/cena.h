/*
 *base abstract class for the screen management
 *define the base loop of rendering
*/


#pragma once
#include <citro2d.h>
#include "Common.h"


class Cena {
public:
    virtual void setStatus(std::string msg) {}
    virtual std::string getIP() { return ""; }
    virtual ~Cena() {} // destrutor
    virtual int update(const InputPacket& packet) = 0;
    virtual void draw(C3D_RenderTarget* top, C3D_RenderTarget* bottom) = 0;
};