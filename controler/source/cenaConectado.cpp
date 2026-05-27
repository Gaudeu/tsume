#include "cenaConectado.h"

inline void interpolar(float& curr, float target, float v, float lim = 0.01f) {
    if (std::abs(curr - target) > lim) {
        curr += (target - curr) * v;
    } else {
        curr = target;
    }
}

cenaConectado::cenaConectado() {
	header = new PainelTopo(25.0f);
	textBuf = C2D_TextBufNew(4096);

	float btnWidth = 120.0f;
	float btnHeight = 40.0f;
	float btnX = (SCREEN_WIDTH_BOTTOM - btnWidth) / 2.0f;
	float btnY = (SCREEN_HEIGHT - btnHeight) / 2.0f;
	std::string textoStatus = "IP: " + ipGlobal + "\nPORTA: " + porta;

	C2D_TextParse(&statusText, textBuf, textoStatus.c_str());
	C2D_TextOptimize(&statusText);
	C2D_TextParse(&voltarText, textBuf, "Back");
	C2D_TextOptimize(&voltarText);
	
	squareColor = C2D_Color32(40, 40, 40, 255);

	botoesC.push_back(Botao(
		20.0f, 
		0.0f, 
		0.4f,
		30.0f, 
		50.0f, 
		nullptr, 
		squareColor, 
		C2D_Color32(200, 200, 200, 255))); //esconder painel
	
    botoesC.push_back(Botao(
		0.0f, 
		235.0f, 
		0.7f,
		0.0f, 
		0.0f, 
		&voltarText, 
		C2D_Color32(255, 0, 0, 255), 
		C2D_Color32(100, 100, 100, 255), 
		20.0f, 
		true));//voltar

	botoesC.push_back(Botao(
		sControl.x + 2.0f,
		-18.0f,
		1.0f,
	    20.0f,
		18.0f, 
		nullptr, 
		C2D_Color32(255, 0, 0, 255), 
		C2D_Color32(0, 0, 0, 255))); // pause

	touchpad = new WidgetDesenho(
		10.0f, 100.0f, 0.5f,
		300.0f, 130.0f,
		C2D_Color32(200, 200, 0, 255),
		C2D_Color32(255, 255, 255, 255)
	);
}

cenaConectado::~cenaConectado() {
	C2D_TextBufDelete(textBuf);
	delete header;
	delete touchpad;
}

void cenaConectado::setStatus(std::string msg) {
	
}

int cenaConectado::update(const InputPacket& packet) {
	if (packet.keysDown & KEY_TOUCH) {
		if (botoesC[0].foiTocado(packet.touchX, packet.touchY)) {
			
			painelAberto = !painelAberto;
			painelAlvo = painelAberto ? 0.0f : -alturaPainel;
		}


		if (botoesC[1].foiTocado(packet.touchX, packet.touchY)) {
			return 3; // Volta para cenaPrincipal
		}
		if (botoesC[2].foiTocado(packet.touchX, packet.touchY)) {
        // pause aqui 
        }

		if(touchpad -> update(packet)){
             // envio pela rede
			 touchpad ->limpar();
			return 4;
		}
	}
	
	interpolar(painelY, painelAlvo, velocidade, 0.1f);

	float margemInterna = 25.0f;
	botoesC[0].setPosicao(20.0f, painelY + margemInterna);
	botoesC[2].setPosicao(sControl.x + 1, painelY + 26.0f);

    float targetIncline = painelAberto ? 3.14159f : 0.0f;

	interpolar(arrowIncline, targetIncline, velocidade, 0.01f);

	//if (packet.keysDown & KEY_B) return 3; // Volta para cenaPrincipal
	return -1;
}

void cenaConectado::draw(C3D_RenderTarget* top, C3D_RenderTarget* bottom) {
	u32 azul = C2D_Color32(0, 0, 255, 255);
	C2D_SceneBegin(top);
	C2D_TargetClear(top, C2D_Color32(255, 255, 255, 255));
	header->draw();

	C2D_DrawRectangle(150.0f, 50.0f, 1.0f, 100.0f, 70.0f, azul, azul, azul, azul);

	C2D_SceneBegin(bottom);
	C2D_TargetClear(bottom, C2D_Color32(255, 255, 255, 255));

	if (painelY >= -alturaPainel){
		
		// desenha o retângulo na tela inferior
		u32 corPainel = C2D_Color32(30, 30, 30, 255); // Cinza escuro

		C2D_DrawRectangle(0.0f, painelY, 0.6f, SCREEN_WIDTH_BOTTOM, alturaPainel,corPainel, corPainel, corPainel, corPainel);

		C2D_DrawText(&statusText, C2D_WithColor, 10.0f, painelY, 1.0f, 0.8f, 0.8f, C2D_Color32(255, 255, 255, 255));

		C2D_DrawRectSolid(sControl.x, painelY + 25.0f, 0.9f, sControl.w, sControl.h, sControl.color);

		
        //seta
		C3D_Mtx Matrix;

		float setaX = 35.0f; // Centralizado horizontalmente
        float setaY = painelY + alturaPainel + 5.5f;

		C2D_ViewSave(&Matrix);

		C2D_ViewTranslate(setaX, setaY);
		C2D_ViewRotate(arrowIncline);

		C2D_DrawTriangle(
        -5.0f, -4.5f, C2D_Color32(0,0,0,255),  // (1.0 - 5.5 = -4.5)
        5.0f, -4.5f, C2D_Color32(0,0,0,255),  // (1.0 - 5.5 = -4.5)
        0.0f,  4.5f, C2D_Color32(0,0,0,255),  // (10.0 - 5.5 = 4.5)
        0.9f               
        );
		//C2D_DrawText(&arrowText, C2D_WithColor | C2D_AlignCenter, 0.0f, 5.0f, 0.7f, 0.8f, 0.8f, C2D_Color32(255, 255, 255, 255));

		C2D_ViewRestore(&Matrix);
	}

    touchpad -> draw();

	for (auto& btn : botoesC) 
		{
        btn.draw();
        }
}

