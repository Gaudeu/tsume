
#pragma once
#include <3ds.h>
#include <string>



extern bool conectado;
extern std::string ipGlobal;
extern std::string porta;

#define SCREEN_WIDTH_TOP    400
#define SCREEN_WIDTH_BOTTOM 320
#define SCREEN_HEIGHT       240

constexpr float BTN_WIDTH = 220.0f;
constexpr float BTN_HEIGHT = 30.0f;

typedef struct {
    float x;
    float y;
    float w;
    float h;
    u32 color;
}placeHolder;

inline float getCenter(float screenWidth, float objectWidth) {
    return (screenWidth - objectWidth) / 2.0f;
}

#pragma pack(push, 1)
struct InputPacket {
    uint32_t keysUp;
    uint32_t keysDown;    // Bot�es pressionados neste frame
    uint32_t keysHeld;    // Bot�es sendo segurados
    int16_t circleX;      // Eixo X do Circle Pad (-150 a 150)
    int16_t circleY;      // Eixo Y do Circle Pad
    uint16_t touchX;      // Coordenada X do Touch (0 a 320)
    uint16_t touchY;      // Coordenada Y do Touch (0 a 240)
};
#pragma pack(pop)
struct Ponto2D
{
    float x;
    float y;
    bool iniciaNovoTraco;
};


