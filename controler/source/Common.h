
#pragma once
#include <3ds.h>
#include <string>



extern int estadoConexao;
extern bool transmissionPaused; 
extern bool isContextPaused;
extern std::string ipGlobal;
extern std::string porta;
extern bool swapAB_XY;
extern bool invertYAxis;
extern int deadZone;

#define SCREEN_WIDTH_TOP    400
#define SCREEN_WIDTH_BOTTOM 320
#define SCREEN_HEIGHT       240

constexpr float BTN_WIDTH = 220.0f;
constexpr float BTN_HEIGHT = 30.0f;

struct placeHolder {
    float x;
    float y;
    float w;
    float h;
    u32 color;
};

inline float getCenter(float screenWidth, float objectWidth) {
    return (screenWidth - objectWidth) / 2.0f;
}
//std::vector?
inline std::string OpenSysKBD(SwkbdType type, const std::string& hintText, int maxLen = 256){
    SwkbdState swkbd;
    char* buffer = new char[maxLen + 1]();

    swkbdInit(&swkbd, type, 2, maxLen ) ;
    swkbdSetHintText(&swkbd, hintText.c_str());

    if (type == SWKBD_TYPE_NUMPAD) {
        swkbdSetNumpadKeys(&swkbd, '.', 0);
    }

    SwkbdButton btnPressed = swkbdInputText(&swkbd, buffer, maxLen + 1);

    std::string result = "";

    if (btnPressed == SWKBD_BUTTON_RIGHT ){
        result = std::string(buffer);
    }

    delete[] buffer;

    return result;
}

#pragma pack(push, 1)
struct InputPacket {
    uint8_t comando;  //0: solicitando, 1: pc disse sim, 2: conectado, 3: falhou, 4 pausado, 6: desconectar
    uint32_t keysUp;
    uint32_t keysDown;    
    uint32_t keysHeld;    
    int16_t circleX;      
    int16_t circleY;      
    uint16_t touchX;      
    uint16_t touchY;    
    
};
#pragma pack(pop)
struct Ponto2D
{
    float x;
    float y;
    bool iniciaNovoTraco;
};


