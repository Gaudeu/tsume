#pragma once
#include "painel.h"

class WidgetBateria : public Painel {
private:
    int nivel = 4; 

public:
    WidgetBateria(float posX, float posY)
  
        : Painel(posX, posY, 1.0f, 34.0f, 18.0f, C2D_Color32(255, 255, 255, 255)) {

       
        for (int i = 0; i < 4; i++) {
          
            float filhoX = (int)posX + 3 + (i * 7); //7px de espaçamento
            float filhoY = (int)posY + 3;

            adicionar(new Widget(filhoX, filhoY, 1.0f, 5.5f, 12.0f,
                C2D_Color32(0, 0, 0, 255), 
                C2D_Color32(0, 0, 0, 255)));
        }
    }

    void setNivel(int n) {
        if (n >= 0 && n <= 4) nivel = n;
    }

    void draw() override {
        u32 corPreta = C2D_Color32(0, 0, 0, 255);

        C2D_DrawRectangle(x - 1.0f, y - 1.0f, 0.5f, w + 2.0f, h + 2.0f, corPreta, corPreta, corPreta, corPreta);//placeholder preto
        C2D_DrawRectangle(x - 3.0f, y + 3.0f, 0.5f, 4.0f, 12.0f, corPreta, corPreta, corPreta, corPreta);//pino

        Widget::draw();

        C2D_DrawRectangle(x - 2.0f, y + 4.0f, 0.5f, 2, 10, corFundo, corFundo, corFundo, corFundo);
        
        for (int i = (4 - nivel); i < 4; i++) {
            filhos[i]->draw();
        }
    }
};