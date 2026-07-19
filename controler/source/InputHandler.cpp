#include "InputHandler.h"
#include "Common.h"
#include <cmath>

int32_t swapButtons(uint32_t keys) {
    
    uint32_t modKeys = keys & ~(KEY_A | KEY_B | KEY_X | KEY_Y); 
    
   
    if (keys & KEY_A) modKeys |= KEY_B;
    if (keys & KEY_B) modKeys |= KEY_A;
    if (keys & KEY_X) modKeys |= KEY_Y;
    if (keys & KEY_Y) modKeys |= KEY_X;
    
    return modKeys;
}

InputPacket InputHandler::scan() {
    hidScanInput(); 

    InputPacket packet{};
    

    //botoes
    uint32_t downRaw = hidKeysDown();
    uint32_t heldRaw = hidKeysHeld();
    uint32_t upRaw = hidKeysUp();

    if (swapAB_XY) {
        packet.keysDown = swapButtons(downRaw);
        packet.keysHeld = swapButtons(heldRaw);
        packet.keysUp   = swapButtons(upRaw);
    } else {
        packet.keysDown = downRaw;
        packet.keysHeld = heldRaw;
        packet.keysUp   = upRaw;
    }
    packet.keysDown = hidKeysDown(); 
        packet.keysHeld = hidKeysHeld(); 
        packet.keysUp = hidKeysUp();



    //cpad
    circlePosition cPos;
    hidCircleRead(&cPos); 

    float magnitude = std::sqrt(cPos.dx * cPos.dx + cPos.dy * cPos.dy);
    if (magnitude < deadZone) {
        packet.circleX = 0;
        packet.circleY = 0;
    } else {
        packet.circleX = cPos.dx;
        packet.circleY = invertYAxis ? -cPos.dy : cPos.dy;
    }
    

    //touchpad
    touchPosition tPos;
    hidTouchRead(&tPos); 
    packet.touchX = tPos.px;
    packet.touchY = tPos.py;

    return packet;
}