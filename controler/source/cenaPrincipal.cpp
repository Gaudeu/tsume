#include "cenaPrincipal.h"

std::string statusConexao = "Aguardando conexão...";


enum botoesMenu {
    BTN_CONECTAR = 0,
	BTN_EDITAR = 1,
	BTN_CONFIG = 2
};

cenaPrincipal::cenaPrincipal() {
    header = new PainelTopo(25.0f);
    rodape = new PainelRodape();

    spriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/Asprite.t3x");
    imagem = C2D_SpriteSheetGetImage(spriteSheet, 0);
    
    textBuf = C2D_TextBufNew(4096);
    dynamicBuf = C2D_TextBufNew(1024);

    C2D_TextParse(&statusText, textBuf, statusConexao.c_str());
    C2D_TextParse(&conectarText, textBuf, "Conectar");
    C2D_TextParse(&editarText, textBuf, "Editar");
    C2D_TextParse(&sairText, textBuf, "Sair");
    C2D_TextParse(&ipText, textBuf, ipGlobal.c_str());
    C2D_TextParse(&portaText, textBuf, porta.c_str());
    C2D_TextParse(&simpleColon, textBuf, ":");
     C2D_TextParse(&msgPopUp, textBuf, "fechar do programa?");
    C2D_TextParse(&txtSim, textBuf, "Sim");
    C2D_TextParse(&txtNao, textBuf, "Não");
    C2D_TextOptimize(&simpleColon);
    C2D_TextOptimize(&portaText);
    C2D_TextOptimize(&ipText);
    C2D_TextOptimize(&statusText);
    C2D_TextOptimize(&conectarText);
    C2D_TextOptimize(&editarText);
    C2D_TextOptimize(&sairText);
    C2D_TextOptimize(&msgPopUp);
    C2D_TextOptimize(&txtSim);
    C2D_TextOptimize(&txtNao); 

    //conteudo dos botoes
    conteudoBotao* txtBtnConectar = new ConteudoTexto(&conectarText);
    conteudoBotao* txtBtnEdit = new ConteudoTexto(&editarText);
    conteudoBotao* txtBtnsair = new ConteudoTexto(&sairText);
    conteudoBotao* txtBtnSim = new ConteudoTexto(&txtSim);
    conteudoBotao* txtBtnNao = new ConteudoTexto(&txtNao);


    btnColor = C2D_Color32(235, 235, 235, 255);
    corTexto = C2D_Color32(0, 255, 0, 255);
	
	centroX = (SCREEN_WIDTH_BOTTOM / 2.0f) - (BTN_WIDTH /2.0f);
    centroY = (SCREEN_HEIGHT / 2.0f) - (BTN_HEIGHT / 2.0f);

	float xBtn = getCenter(SCREEN_WIDTH_BOTTOM, BTN_WIDTH);

    float btnPopUpWidth = (cPopUp.w/2);
    float btnPopUpHeight = 30.0f;


    botoes.push_back(Botao(
        xBtn, 
        centroY - 50, 
        0.5f,
        BTN_WIDTH, 
        BTN_HEIGHT, 
        txtBtnConectar, 
        btnColor, 
        C2D_Color32(200, 200, 200, 255)));

    botoes.push_back(Botao(
        xBtn,
        centroY - 10, 
        0.5f,
        BTN_WIDTH, 
        BTN_HEIGHT, 
        txtBtnEdit, 
        btnColor, 
        C2D_Color32(200, 200, 200, 255)));

    botoes.push_back(Botao(
        xBtn, 
        centroY + 30, 
        0.5f,
        BTN_WIDTH, 
        BTN_HEIGHT, 
        txtBtnsair, 
        btnColor, 
        C2D_Color32(200, 200, 200, 255)));

    botoes.push_back(Botao(
		0.0f, 
		235.0f, 
		0.7f,
		0.0f, 
		0.0f, 
		nullptr, 
		C2D_Color32(255, 0, 0, 255), 
		C2D_Color32(100, 100, 100, 255), 
		20.0f, 
		true));

    botoes[0].selecionado = true;

    botoesPopUp.push_back(Botao(
        cPopUp.x, 
        cPopUp.y + 40.0f, 
        0.5f,
        btnPopUpWidth, 
        btnPopUpHeight, 
        txtBtnSim,
        C2D_Color32(190, 190, 190, 255), 
        C2D_Color32(160, 160, 160, 255)));

    botoesPopUp.push_back(Botao(
        cPopUp.x + btnPopUpWidth, 
        cPopUp.y + 40.0f, 
        0.5f,
        btnPopUpWidth, 
        btnPopUpHeight, 
        txtBtnNao, 
        C2D_Color32(190, 190, 190, 255), 
        C2D_Color32(160, 160, 160, 255)));
    
    botoesPopUp[1].selecionado = true;


}

cenaPrincipal::~cenaPrincipal() {
    C2D_SpriteSheetFree(spriteSheet);
    C2D_TextBufDelete(textBuf);
    C2D_TextBufDelete(dynamicBuf);
    delete rodape;
	delete header;
}

void cenaPrincipal::setStatus(std::string msg) {
	C2D_TextBufClear(dynamicBuf);

    C2D_TextParse(&statusText, dynamicBuf, msg.c_str());

    C2D_TextOptimize(&statusText);
}






int cenaPrincipal::update(const InputPacket& packet) { 
    
    
    if (exibirPopUp) {
        if (packet.keysDown & KEY_DLEFT) {
            botoesPopUp[indiceFocoPopUp].selecionado = false;
            indiceFocoPopUp = (indiceFocoPopUp - 1 + (int)botoesPopUp.size()) % botoesPopUp.size();
            botoesPopUp[indiceFocoPopUp].selecionado = true;
        }
        else if (packet.keysDown & KEY_DRIGHT) {
            botoesPopUp[indiceFocoPopUp].selecionado = false;
            indiceFocoPopUp = (indiceFocoPopUp + 1) % botoesPopUp.size();
            botoesPopUp[indiceFocoPopUp].selecionado = true;
        }

        
        if (packet.keysDown & KEY_A) {
            if (indiceFocoPopUp == 0) { 
                return 2; 
            } else { 
                exibirPopUp = false; 
            }
        }
        
        
        if (packet.keysDown & KEY_B) {
            exibirPopUp = false;
        }

        return -1; 
    }

    int numBotoesNavegaveis = botoes.size() - 1;

    if (packet.keysDown & KEY_DDOWN) {
        botoes[indiceFoco].selecionado = false;
        indiceFoco = (indiceFoco + 1) % numBotoesNavegaveis;
        botoes[indiceFoco].selecionado = true;
    }
    else if (packet.keysDown & KEY_DUP) {
        botoes[indiceFoco].selecionado = false;
        indiceFoco = (indiceFoco - 1 + numBotoesNavegaveis) % numBotoesNavegaveis;
        botoes[indiceFoco].selecionado = true;
    }

    
    if (packet.keysDown & KEY_A) {
        if (indiceFoco == BTN_CONECTAR) { 
            return 0; 
        }
        if (indiceFoco == BTN_EDITAR) { 
            return 1; 
        }
        if (indiceFoco == BTN_CONFIG) { 
            exibirPopUp = true;         
            return -1;                  
        }
    }

    return -1;
}

void cenaPrincipal::draw(C3D_RenderTarget* top, C3D_RenderTarget* bottom) {
    // TELA SUPERIOR 
    float escalaTexto = 0.6f;
    float meio140 = (SCREEN_WIDTH_TOP / 2.0f) - (65);// 65 � metade da largura do container
    float meio120 = (SCREEN_WIDTH_TOP / 2.0f) - (60);
    float ipX = 145.0f;         // meio do container + um pequeno padding
    float ipY = 105.0f;         // Ajustado para dentro do ret�ngulo
    C2D_SceneBegin(top);
    C2D_TargetClear(top, C2D_Color32(0, 0, 0, 255));
 
    C2D_DrawImageAt(imagem, 40.0f, 0, 0.3f, NULL, 1.0f, 1.0f);
    
	
    C2D_DrawRectangle(meio140, 95.0f, 0.8f, 180.0f, 40.0f, C2D_Color32(0, 0, 0, 255), C2D_Color32(0, 0, 0, 255), C2D_Color32(0, 0, 0, 255), C2D_Color32(0, 0, 0, 255));
    C2D_DrawRectangle(meio120, 100.0f, 0.9f, 170.0f, 30.0f, btnColor, btnColor, btnColor, btnColor);

    float larguraIP, alturaIP;
    C2D_TextGetDimensions(&ipText, escalaTexto, escalaTexto, &larguraIP, &alturaIP);

    C2D_DrawText(&ipText, C2D_WithColor, ipX, ipY, 1.0f, escalaTexto, escalaTexto, corTexto);
    
    float portaX = ipX + larguraIP;
    C2D_DrawText(&simpleColon, C2D_WithColor, portaX, ipY, 1.0f, escalaTexto, escalaTexto, corTexto);
    C2D_DrawText(&portaText, C2D_WithColor, (portaX+2.0f), ipY, 1.0f, escalaTexto, escalaTexto, corTexto);
    
    //
    C2D_DrawImageAt(imagem, 40.0f, 0, 0.3f, NULL, 1.0f, 1.0f);
    C2D_DrawText(&statusText, C2D_AtBaseline, 100.0f, 180.0f, 1.0f, 0.6f, 0.6f);
    

    header->draw();
    
    // TELA INFERIOR
    C2D_SceneBegin(bottom);
    C2D_TargetClear(bottom, C2D_Color32(255, 255, 255, 255));

    rodape->draw();


    if(!exibirPopUp)
    {
      for (auto& btn : botoes) {
        btn.draw();
    }
    } else 
    {
    C2D_DrawRectSolid(cPopUp.x, cPopUp.y, 0.4f, cPopUp.w, cPopUp.h, cPopUp.color);
        C2D_DrawText(&msgPopUp, C2D_AtBaseline, cPopUp.x + 21.0f, cPopUp.y + 30.0f, 1.0f, 0.6f, 0.6f);
        
        for (auto& btnPop : botoesPopUp) {
            btnPop.draw();
        }    
    }

}

