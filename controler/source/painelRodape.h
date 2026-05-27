#pragma once
#include "painel.h"
#include <string>

class PainelRodape : public Painel {
private:
    C2D_TextBuf bufferTexto;
    C2D_Text textoRodape;

public:
    // O texto padrão já é o que você pediu. A altura padrão é 25 pixels.
    PainelRodape(std::string texto = "A: Selecionar   B: Voltar", float altura = 25.0f)
        // Posicionamento: x=0, y=(240 - altura), largura=320 (tela inferior)
        : Painel(0.0f, 240.0f - altura, 0.4f, 320.0f, altura, C2D_Color32(0, 255, 255, 255)) {

        // 1. Inicializa o buffer do Citro2D (vital para renderizar texto)
        bufferTexto = C2D_TextBufNew(64);

        // 2. Transforma a std::string no formato C2D_Text
        C2D_TextParse(&textoRodape, bufferTexto, texto.c_str());
        C2D_TextOptimize(&textoRodape);
    }

    ~PainelRodape() {
        // Limpeza obrigatória para evitar vazamento de memória (memory leak) ao fechar a cena
        C2D_TextBufDelete(bufferTexto);
    }

    void draw() override {
        
        Painel::draw();

        
        C2D_DrawText(&textoRodape, C2D_WithColor, x + 10.0f, y + (h / 2.0f) - 8.0f,
            0.5f, 0.5f, 0.5f, C2D_Color32(0, 0, 0, 255));
    }
};