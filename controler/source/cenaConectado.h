#pragma once
#include "cena.h"
#include <3ds.h>
#include "botao.h"
#include <vector>
#include "painelTopo.h"
#include "WidgetDesenho.h"

class cenaConectado : public Cena {
private:
	PainelTopo* header;
    
	C2D_SpriteSheet spriteSheetPen, spriteSheetPenOff, spriteSheetPlay, spriteSheetPause;
    C2D_Image imgPen, imgPenOff, imgPlay, imgPause;

	C2D_TextBuf textBuf;
	C2D_Text voltarText;
	C2D_Text infoText, statusText;
	
	float arrowIncline;

	u32 fieldColor, squareColor;
	std::vector<Botao*> botoesC;
	std::vector<conteudoBotao*> conteudosBotoes;

	bool exibirRetangulo = false;
	bool drawTouchpad = false;
	bool processInThisFrame = false;
	//dinamica do menu
	bool painelAberto = false;
	float painelY = -60.0f;      // Começa fora da tela (ajuste conforme a altura do painel)
	float painelAlvo = -60.0f;   // Onde o painel quer chegar
	float alturaPainel = 60.0f;  
	float velocidade = 0.15f;

	placeHolder sControl = { 
		.x = 200.0f,
		.y = -18.0f,
		.w = 90.0f,
		.h = 20.0f,
		.color = C2D_Color32(255, 255, 255, 255)
	};


public:
	void setStatus(std::string msg) override;
	cenaConectado();
	~cenaConectado();
	int update(const InputPacket& packet) override;
	void draw(C3D_RenderTarget* top, C3D_RenderTarget* bottom) override;
	WidgetDesenho* touchpad;
};