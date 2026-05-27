#pragma once
#include "cena.h"
#include <3ds.h>
#include "botao.h"
#include <vector>
#include "painelTopo.h"
#include "painelRodape.h"



class cenaTexto : public Cena {
private:
    C2D_TextBuf staticBuf;
    C2D_Text infoText;
    C2D_Text botaoTextIP, botaoTextPort, botaoConfirm;
    C2D_Text parametersText;
    u32 squareColor, fieldColor;
    std::vector<Botao> botoes;
    int indiceFoco = 0;

    std::string ipTemp;
    std::string portaTemp;

    PainelTopo* header;
    PainelRodape* rodape;
    


public:
    std::string ip;
    std::string getIP() override { return ip; }
    cenaTexto();
    ~cenaTexto(); // Para limpar o buffer de texto
    int update(const InputPacket& packet) override;
    void draw(C3D_RenderTarget* top, C3D_RenderTarget* bottom) override;
    std::string abrirTecladoIP();
    std::string abrirTecladoPorta();
};