#include "InputHandler.h"

InputPacket InputHandler::scan() {
    hidScanInput(); 

        InputPacket packet;
    packet.keysDown = hidKeysDown(); 
        packet.keysHeld = hidKeysHeld(); 

        circlePosition cPos;
    hidCircleRead(&cPos); 
        packet.circleX = cPos.dx;
    packet.circleY = cPos.dy;

    touchPosition tPos;
    hidTouchRead(&tPos); 
    packet.touchX = tPos.px;
    packet.touchY = tPos.py;

    return packet;
}