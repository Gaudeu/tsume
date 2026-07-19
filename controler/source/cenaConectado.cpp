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
	dynamicBuf = C2D_TextBufNew(1024);

	C2D_TextParse(&msgPopUp, textBuf, "Doing this will end the session \n are you sure?");
	C2D_TextParse(&txtYes, textBuf, "Yes");
	C2D_TextParse(&txtNo, textBuf, "No");
	C2D_TextParse(&txtHelpTouchpad, textBuf, "A:Send  B:Close  X:Undo  Y:Clear");
	C2D_TextOptimize(&msgPopUp);
	C2D_TextOptimize(&txtYes);
	C2D_TextOptimize(&txtNo);
	C2D_TextOptimize(&txtHelpTouchpad);

	spriteSheetPen = C2D_SpriteSheetLoad("romfs:/gfx/Bsprite.t3x");
	imgPen = C2D_SpriteSheetGetImage(spriteSheetPen, 0);

	spriteSheetPenOff = C2D_SpriteSheetLoad("romfs:/gfx/Csprite.t3x");
	imgPenOff = C2D_SpriteSheetGetImage(spriteSheetPenOff, 0);

	spriteSheetPlay = C2D_SpriteSheetLoad("romfs:/gfx/Dsprite.t3x");
	imgPlay = C2D_SpriteSheetGetImage(spriteSheetPlay, 0);

	spriteSheetPause = C2D_SpriteSheetLoad("romfs:/gfx/Esprite.t3x");
	imgPause = C2D_SpriteSheetGetImage(spriteSheetPause, 0);


	

    //conteudo botoes
	conteudoBotao* conteudoBotaoPen = new ConteudoImagem(&imgPen, &imgPenOff);
    conteudosBotoes.push_back(conteudoBotaoPen);

	conteudoBotao* conteudoBotaoPlay = new ConteudoImagem(&imgPlay, &imgPause);
    conteudosBotoes.push_back(conteudoBotaoPlay);

	//conteudo botoes popup
	conteudoBotao* txtBtnYes = new ConteudoTexto(&txtYes);
	conteudoBotao* txtBtnNo = new ConteudoTexto(&txtNo);
	conteudosBotoes.push_back(txtBtnYes);
	conteudosBotoes.push_back(txtBtnNo);

	float btnPopUpWidth = (cPopUp.w / 2);
	float btnPopUpHeight = 30.0f;


	setStatus("Conectado");

	float btnWidth = 120.0f;
	float btnHeight = 40.0f;
	float btnX = (SCREEN_WIDTH_BOTTOM - btnWidth) / 2.0f;
	float btnY = (SCREEN_HEIGHT - btnHeight) / 2.0f;
	std::string parametersText = "IP: " + ipGlobal + "\nPORTA: " + porta;

	C2D_TextParse(&PText, textBuf, parametersText.c_str());
	C2D_TextOptimize(&PText);


	popUpButtons.push_back(Botao(
		cPopUp.x, 
		cPopUp.y + 60.0f, 
		0.5f, 
		btnPopUpWidth, 
		btnPopUpHeight, 
		txtBtnYes,
		C2D_Color32(190, 190, 190, 255), 
		C2D_Color32(160, 160, 160, 255)));

	popUpButtons.push_back(Botao(
		cPopUp.x + btnPopUpWidth, 
		cPopUp.y + 60.0f, 
		0.5f,
		btnPopUpWidth, 
		btnPopUpHeight, 
		txtBtnNo, 
		C2D_Color32(190, 190, 190, 255), 
		C2D_Color32(160, 160, 160, 255)));
    
	popUpButtons[1].selecionado = true;

	squareColor = C2D_Color32(40, 40, 40, 255);

	botoesC.push_back(new Botao(
		20.0f, 
		0.0f, 
		0.4f,
		30.0f, 
		50.0f, 
		nullptr, 
		squareColor, 
		C2D_Color32(200, 200, 200, 255))); //esconder painel (indice 0)
	
    botoesC.push_back(new Botao(
		0.0f, 
		235.0f, 
		0.7f,
		0.0f, 
		0.0f, 
		nullptr, 
		C2D_Color32(255, 0, 0, 255), 
		C2D_Color32(100, 100, 100, 255), 
		20.0f, 
		true));//voltar (indice 1)

	botoesC.push_back(new Botao(
		sControl.x + 2.0f,
		-18.0f,
		0.9f,
	    20.0f,
		18.0f, 
		conteudoBotaoPlay, 
		C2D_Color32(0, 255, 0, 255), 
		C2D_Color32(0, 0, 0, 255))); // pause (indice 2)

	botoesC.push_back(new Botao(
		sControl.x + 24.0f,
		-18.0f,
		0.9f,
	    20.0f,
		18.0f, 
		conteudoBotaoPen, 
		C2D_Color32(0, 0, 255, 255), 
		C2D_Color32(0, 0, 0, 255))); //exibir touch pad (indice 3)

		botoesC.push_back(new Botao(
		sControl.x + 46.0f,
		-18.0f,
		0.9f,
	    20.0f,
		18.0f, 
		nullptr, 
		C2D_Color32(0, 0, 255, 255), 
		C2D_Color32(0, 0, 0, 255))); //exibir teclado (indice 4)

	touchpad = new WidgetDesenho(
		10.0f, 80.0f, 0.5f,
		300.0f, 130.0f,
		C2D_Color32(200, 200, 200, 255),
		C2D_Color32(0, 0, 0, 255)
	);
}

cenaConectado::~cenaConectado() {
	C2D_TextBufDelete(textBuf);
	C2D_TextBufDelete(dynamicBuf);
	delete header;
	delete touchpad;

	for (auto btn : botoesC) {
        delete btn;
    }

	for (auto cont : conteudosBotoes) {
        delete cont;
    }

	C2D_SpriteSheetFree(spriteSheetPen);
    C2D_SpriteSheetFree(spriteSheetPenOff);
	C2D_SpriteSheetFree(spriteSheetPlay);   
    C2D_SpriteSheetFree(spriteSheetPause);
}

void cenaConectado::setStatus(std::string msg) {
	if (textBuf != nullptr) {
		C2D_TextBufClear(dynamicBuf);
        
        // Recria o texto com a string atualizada com segurança
        C2D_TextParse(&statusText, dynamicBuf, msg.c_str());
        C2D_TextOptimize(&statusText);
        
    }
}

int cenaConectado::update(const InputPacket& packet) {

	//popup control
    if(showPopUp){
		if(packet.keysDown & KEY_DLEFT){
			popUpButtons[popUpIndex].selecionado = false;
			popUpIndex = (popUpIndex - 1 + (int)popUpButtons.size()) % popUpButtons.size(); 
			popUpButtons[popUpIndex].selecionado = true;
		} else if(packet.keysDown & KEY_DRIGHT){
            popUpButtons[popUpIndex].selecionado = false;
			popUpIndex = (popUpIndex + 1 ) % popUpButtons.size(); 
			popUpButtons[popUpIndex].selecionado = true;
		}

		if (packet.keysDown & KEY_A) {
			if (popUpIndex == 0) { 
				return 3;
			} else { 
				showPopUp = false; 
			}
		}

		if (packet.keysDown & KEY_TOUCH) {
			if (popUpButtons[0].foiTocado(packet.touchX, packet.touchY)) return 3;
			if (popUpButtons[1].foiTocado(packet.touchX, packet.touchY)) showPopUp = false;
		}

		
		if (packet.keysDown & KEY_B) {
			showPopUp = false;
		}

		return -1;
	}
    //

	//widgetdesenho dinamica
	if (drawTouchpad && touchpad != nullptr) {
		
		
		if (touchpad->update(packet)) {
			processInThisFrame = true;
		}

		
		if (packet.keysDown & KEY_A) {
			if (touchpad->forceSend()) {
				processInThisFrame = true;
			}
		}

		
		if (packet.keysDown & KEY_B) {
			drawTouchpad = false;
			botoesC[3]->aoClicar();
		}

		if (packet.keysDown & KEY_TOUCH) {
			
			if (botoesC[3]->foiTocado(packet.touchX, packet.touchY)) {
				drawTouchpad = false;
				botoesC[3]->aoClicar(); // Alterna a imagem
				return -1; 
			}
		}

		
		if (packet.keysDown & KEY_X) {
			touchpad->undo();
		}

		
		if (packet.keysDown & KEY_Y) {
			touchpad->limpar();
		}

		
		if (processInThisFrame) {
			processInThisFrame = false;
			return 10; 
		}

		return -1; 
	}
    //

	if (packet.keysDown & KEY_TOUCH) {
		if (botoesC[0]->foiTocado(packet.touchX, packet.touchY)) {
			
			painelAberto = !painelAberto;
			painelAlvo = painelAberto ? 0.0f : -alturaPainel;
		}


		if (botoesC[1]->foiTocado(packet.touchX, packet.touchY)) {
			showPopUp = true;
			popUpButtons[0].selecionado = false;
			popUpButtons[1].selecionado = true;
			popUpIndex = 1;
		}
		if (botoesC[2]->foiTocado(packet.touchX, packet.touchY)) {
        botoesC[2]->aoClicar(); 
        return 11;
        }
		if (botoesC[3]->foiTocado(packet.touchX, packet.touchY)) {
        
		botoesC[3]->aoClicar();
		drawTouchpad = !drawTouchpad;
        }
		if (botoesC[4]->foiTocado(packet.touchX, packet.touchY)) {
        
		botoesC[4]->aoClicar();
		drawTouchpad = !drawTouchpad;

		std::string textTyped = OpenSysKBD(SWKBD_TYPE_NORMAL, "...");

		if(!textTyped.empty()){
			textToDeliver = textTyped;

			return 12;
		}
        }

	}

	/*
	
	if(touchpad != nullptr){

		if(touchpad -> update(packet)){
             // envio pela rede
			 touchpad ->limpar();
			return 10;
		}
	}
	
	
    if(drawTouchpad && touchpad != nullptr){
		if(touchpad->update(packet)){
			processInThisFrame = true;
		}
	}
	*/
	if(processInThisFrame){
		processInThisFrame = false;
		return 10;
	}

	
	interpolar(painelY, painelAlvo, velocidade, 0.1f);

	float margemInterna = 25.0f;
	botoesC[0]->setPosicao(20.0f, painelY + margemInterna);
	botoesC[2]->setPosicao(sControl.x + 1, painelY + 26.0f);
	botoesC[3]->setPosicao(sControl.x + 22, painelY + 26.0f);
	botoesC[4]->setPosicao(sControl.x + 43, painelY + 26.0f);
	//spritePencil.setPosicao(sControl.x + 1, painelY + 26.0f);


    float targetIncline = painelAberto ? 3.14159f : 0.0f;

	interpolar(arrowIncline, targetIncline, velocidade, 0.01f);

	//if (packet.keysDown & KEY_B) return 3; 
	return -1;
}

void cenaConectado::draw(C3D_RenderTarget* top, C3D_RenderTarget* bottom) {
	u32 azul = C2D_Color32(0, 0, 255, 255);
	C2D_SceneBegin(top);
	C2D_TargetClear(top, C2D_Color32(255, 255, 255, 255));
	C2D_ViewReset();
	header->draw();

	C2D_DrawText(&statusText, C2D_WithColor, 100.0f, 100.0f, 1.0f, 0.8f, 0.8f, C2D_Color32(0, 0, 0, 255));


	//C2D_DrawRectangle(150.0f, 50.0f, 1.0f, 100.0f, 70.0f, azul, azul, azul, azul);

	C2D_SceneBegin(bottom);
	C2D_TargetClear(bottom, C2D_Color32(255, 255, 255, 255));
	C2D_ViewReset();

	


	C2D_DrawRectangle(0.0f, 214.0f, 0.1f, SCREEN_WIDTH_BOTTOM, 30.0f, C2D_Color32(200, 200, 200, 255), C2D_Color32(200, 200, 200, 255), C2D_Color32(200, 200, 200, 255), C2D_Color32(200, 200, 200, 255));

	if (painelY >= -alturaPainel){
		
		// retângulo azul
		u32 corPainel = C2D_Color32(30, 30, 30, 255); // Cinza escuro

		C2D_DrawRectangle(0.0f, painelY, 0.6f, SCREEN_WIDTH_BOTTOM, alturaPainel,corPainel, corPainel, corPainel, corPainel);

		C2D_DrawText(&PText, C2D_WithColor, 10.0f, painelY, 1.0f, 0.8f, 0.8f, C2D_Color32(255, 255, 255, 255));

		C2D_DrawRectSolid(sControl.x, painelY + 25.0f, 0.9f, sControl.w, sControl.h, sControl.color);

		
        //seta
		C3D_Mtx Matrix;

		float setaX = 35.0f; 
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

	if (drawTouchpad && touchpad != nullptr ){
		touchpad -> draw();
		C2D_DrawText(&txtHelpTouchpad, C2D_WithColor, 19.0f, 222.0f, 0.5f, 0.55f, 0.55f, C2D_Color32(0, 0, 0, 255));
	}



	for (auto& btn : botoesC) 
	{
        btn->draw();
    }

	if (showPopUp) {
		C2D_DrawRectSolid(cPopUp.x, cPopUp.y, 0.4f, cPopUp.w, cPopUp.h, cPopUp.color);
		C2D_DrawText(&msgPopUp, C2D_AtBaseline, cPopUp.x + 15.0f, cPopUp.y + 30.0f, 0.5f, 0.6f, 0.6f, C2D_Color32(0,0,0,255));
		
		for (auto& btnPop : popUpButtons) {
			btnPop.draw();
		}
	}//====================FALTA PAUSAR A EMULAÇAO QUANDO O POPUP APARECER====================
}

