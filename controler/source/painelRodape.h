#pragma once
#include "painel.h"
#include <string>

class PainelRodape : public Painel {
private:
    C2D_TextBuf bufferTexto;
    C2D_Text textoRodape;

public:

    PainelRodape(std::string texto = "A: select   B: return", float altura = 25.0f)
        
        : Painel(0.0f, 240.0f - altura, 0.4f, 320.0f, altura, C2D_Color32(150, 150, 150, 255)) {

        
        bufferTexto = C2D_TextBufNew(64);

        
        C2D_TextParse(&textoRodape, bufferTexto, texto.c_str());
        C2D_TextOptimize(&textoRodape);
    }

    ~PainelRodape() {
        
        C2D_TextBufDelete(bufferTexto);
    }

    void draw() override {
        
        Painel::draw();

        
        C2D_DrawText(&textoRodape, C2D_WithColor, x + 30.0f, y + (h / 2.0f) - 8.0f,
            0.5f, 0.5f, 0.5f, C2D_Color32(0, 0, 0, 255));
    }
};