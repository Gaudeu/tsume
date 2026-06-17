#pragma once
#include "painel.h"
#include "WidgetBateria.h"

class PainelTopo : public Painel {
private:
    WidgetBateria* bateria;
    int nivelWifi = 3;

public:
   
    PainelTopo(float altura = 30.0f)
        : Painel(0.0f, 0.0f, 0.4f, 400.0f, altura, C2D_Color32(150, 150, 150, 255)) {

      
        bateria = new WidgetBateria(w - 50.0f, 4.0f);
        adicionar(bateria);
        
    }

    void atualizarBateria(int x) {
        bateria->setNivel(x);
    }

    void setNivelWifi(int n) {
        if (n >= 0 && n <= 3) nivelWifi = n;
    }
    
    void draw() override {

        u8 wifiSTR = osGetWifiStrength();
        setNivelWifi(wifiSTR);

        u8 bateriaSTR = 0;
        PTMU_GetBatteryLevel(&bateriaSTR);
        atualizarBateria((int)bateriaSTR - 1);
        
        Painel::draw();

       
        u32 corLinha = C2D_Color32(0, 0, 0, 255);
        u32 corVermelho = C2D_Color32(255, 0, 0, 255);
		u32 corVerde = C2D_Color32(0, 255, 0, 255);
        float posX =  30.0f;
        float margemInferior = 5.0f;
        float base = h - margemInferior;

		if (nivelWifi == 0) {
            float tamanhoSq = 10.0f;
            C2D_DrawRectangle(x + posX, y + base - tamanhoSq, 0.5f, tamanhoSq, tamanhoSq,
                corLinha, corLinha, corLinha, corLinha);
        }
        else {
            desenharLinha(posX, base - 7.0f, posX, base, 3.5f, corLinha);
            C2D_DrawCircle(posX, base - 11.0f, 1.0f, 3.5f, corLinha, corLinha, corLinha, corLinha);

            if (nivelWifi >= 1) {
                desenharLinha(posX + 6.0f, base - 4.0f, posX + 6.0f, base, 3.5f, corLinha);
            }
            if (nivelWifi >= 2) {
                desenharLinha(posX + 12.0f, base - 8.0f, posX + 12.0f, base, 3.5f, corLinha);
            }
            if (nivelWifi >= 3) {
                desenharLinha(posX + 18.0f, base - 12.0f, posX + 18.0f, base, 3.5f, corLinha);
            }
        }

        

        // 3. RETÂNGULO VERMELHO À DIREITA
        // Posicionado 10 pixels após a última linha possível (18 + 10 = 28)
        // Usamos x + posX para garantir que ele siga o painel
        if (estadoConexao == 2) {
            C2D_DrawRectangle(x + posX + 28.0f, y + base - 14.0f, 0.5f, 6.0f, 14.0f,
                corVerde, corVerde, corVerde, corVerde);
        }
        else {
            C2D_DrawRectangle(x + posX + 28.0f, y + base - 14.0f, 0.5f, 6.0f, 14.0f,
                corVermelho, corVermelho, corVermelho, corVermelho);
        }
        
    }
};