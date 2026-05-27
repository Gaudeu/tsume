#pragma once
#include "cena.h"

#include "botao.h"
#include "painelTopo.h"
#include "painelRodape.h"
#include <vector>


class cenaPrincipal : public Cena {
private:
    PainelTopo* header;
    PainelRodape* rodape;

    C2D_SpriteSheet spriteSheet;
    C2D_Image imagem;
    C2D_TextBuf textBuf;
    C2D_TextBuf dynamicBuf;
    C2D_Text statusText, conectarText, editarText, sairText;
	C2D_Text ipText, portaText, simpleColon;
    C2D_Text msgPopUp, txtSim, txtNao;
    std::string statusConexao = "Aguardando conexão...";
    u32 btnColor, corTexto;
    //float btnWidth, btnHeight;
	float centroX, centroY;
    std::vector<Botao> botoes;
    int indiceFoco = 0;
    std::vector<Botao> botoesPopUp;
    int indiceFocoPopUp = 0;
    bool exibirPopUp = false;

    placeHolder cPopUp = { 
		.x = 60.0f,
		.y = 70.0f,
		.w = 200.0f,
		.h = 70.0f,
		.color = C2D_Color32(240, 240, 240, 255)
	};
    


public:
    void setStatus(std::string msg) override;
    cenaPrincipal();
    ~cenaPrincipal();
    int update(const InputPacket& packet) override;
    void draw(C3D_RenderTarget* top, C3D_RenderTarget* bottom) override;
};