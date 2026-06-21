#include "cenaTexto.h"
#include "saveConfig.h"
const char* altIP = "alterar IP /";
const char* altPort = "alterar Porta /";
bool ok = false;
float parameterField = 190.0f;
float xBtnBot = getCenter(SCREEN_WIDTH_BOTTOM, BTN_WIDTH);
//float xBtnTop = getCenter(SCREEN_WIDTH_TOP, parameterField);
float escalaTexto = 0.8f;

float deadzoneValue = 0.0f;



cenaTexto::cenaTexto() {
    header = new PainelTopo(25.0f);
    rodape = new PainelRodape("A: selecionar  B: Voltar  Y: salvar");

    char buffer[10];
    snprintf(buffer, sizeof(buffer), "%.1f", deadzoneValue);
    std::string deadzoneString = buffer;

    staticBuf = C2D_TextBufNew(4096);

    ipTemp = ipGlobal;
    portaTemp = porta;
    std::string displayParam = ipTemp + ":" + portaTemp;

    struct { C2D_Text* obj; const char* texto; } textos[] = {
    {&ipText, ipTemp.c_str()},
    {&portaText, portaTemp.c_str()},
    {&deadzoneText, deadzoneString.c_str()},
    {&infoText,       "[IP:PORT]"},
    {&parametersText, displayParam.c_str()},
    {&botaoTextIP,    altIP},
    {&botaoTextPort,  altPort},
    {&botaoConfirm,   "Ok"},
    {&seta,           ">"},
    {&help, "?"},
    {&deadzone, "DeadZone do \n analogico/"},
    {&invertAxisY, "inverter eixo Y /"},
    {&emularParaQualStick, "Cpad será emulado para qual analogico? /"},
    {&swapABandXY, "trocar os botoes XY e AB? /"},
    {&clearTop, "escurecer a tela superior enquanto conectado? /"}
    };

    for (const auto& item : textos) {
    C2D_TextParse(item.obj, staticBuf, item.texto);
    C2D_TextOptimize(item.obj);
    }


	squareColor = C2D_Color32(255, 255, 255, 255); 
    fieldColor = C2D_Color32(240, 240, 240, 255);
    float centroX = (SCREEN_WIDTH_BOTTOM / 2.0f) - (BTN_WIDTH / 2.0f);
    float centroY = (SCREEN_HEIGHT / 2.0f) - (BTN_HEIGHT / 2.0f);


    
    // pagina  0
    std::vector<Botao> pagina0;
    pagina0.push_back(Botao(20, 
        centroY - 55, 
        0.5f, 
        280, 
        BTN_HEIGHT, 
        new ConteudoTexto(&botaoTextIP, 0.6, false), 
        squareColor, 
        C2D_Color32(200, 200, 200, 255)));
    pagina0.push_back(Botao(20, 
        centroY - 15, 
        0.5f, 
        280, 
        BTN_HEIGHT, 
        new ConteudoTexto(&botaoTextPort, 0.6, false), 
        squareColor, 
        C2D_Color32(200, 200, 200, 255)));
    pagina0.push_back(Botao(20, 
        centroY + 25, 
        0.5f, 
        280, 
        BTN_HEIGHT, 
        new ConteudoTexto(&deadzone, 0.6, false), 
        squareColor, 
        C2D_Color32(200, 200, 200, 255)));

    pagina0.push_back(Botao(20, 
        centroY + 65, 
        0.5f, 
        280, 
        BTN_HEIGHT, 
        new ConteudoTexto(&invertAxisY, 0.6, false), 
        squareColor, 
        C2D_Color32(200, 200, 200, 255))); 

    //
    //pagina 1
        
    std::vector<Botao> pagina1;
    pagina1.push_back(Botao(20, 
        centroY - 55, 
        0.5f, 
        280, 
        BTN_HEIGHT, 
        new ConteudoTexto(&emularParaQualStick, 0.4, false), 
        squareColor, 
        C2D_Color32(200, 200, 200, 255)));
    pagina1.push_back(Botao(20, 
        centroY - 15, 
        0.5f, 
        280, 
        BTN_HEIGHT, 
        new ConteudoTexto(&swapABandXY, 0.6, false), 
        squareColor, 
        C2D_Color32(200, 200, 200, 255)));
    pagina1.push_back(Botao(20, 
        centroY + 25, 
        0.5f, 
        280, 
        BTN_HEIGHT, 
        new ConteudoTexto(&clearTop, 0.4, false), 
        squareColor, 
        C2D_Color32(200, 200, 200, 255)));
    


    paginasBotoes.push_back(pagina0);
    paginasBotoes.push_back(pagina1);

    //botoes de avulsos
    botoes.push_back(Botao(310, 
        centroY - 30, 
        0.5f, 
        10, 
        60, 
        new ConteudoTexto(&seta), 
        squareColor, 
        C2D_Color32(200, 200, 200, 255)));
    
    botoes.push_back(Botao(0, 
        centroY - 30, 
        0.5f, 
        10, 
        60, 
        new ConteudoTexto(&seta), 
        squareColor, 
        C2D_Color32(200, 200, 200, 255)));

        botoes.push_back(Botao(250, 
        20, 
        0.5f, 
        40, 
        15, 
        new ConteudoTexto(&help), 
        squareColor, 
        C2D_Color32(200, 200, 200, 255)));

        botoes.push_back(Botao(205, 
        20, 
        0.5f, 
        40, 
        15, 
        new ConteudoTexto(&botaoConfirm), 
        squareColor, 
        C2D_Color32(200, 200, 200, 255)));
    //
    paginasBotoes[0][0].selecionado = true;
}

cenaTexto::~cenaTexto() {
    C2D_TextBufDelete(staticBuf);
    delete rodape;
    delete header;
}
// TECLADOS
std::string cenaTexto::abrirTecladoIP() {
    SwkbdState swkbd;
    char buffer[16];

    swkbdInit(&swkbd, SWKBD_TYPE_NUMPAD, 2, 15);

    swkbdSetHintText(&swkbd, "Digite o IP do PC (ex: 192.168.0.10)");

    swkbdSetNumpadKeys(&swkbd, '.', 0);

    SwkbdButton botaoPressionado = swkbdInputText(&swkbd, buffer, sizeof(buffer));

    if (botaoPressionado == SWKBD_BUTTON_RIGHT) {
        return std::string(buffer);
    }

    return "";
}
std::string cenaTexto::abrirTecladoPorta() {
    SwkbdState swkbd;
    char buffer[16];

    swkbdInit(&swkbd, SWKBD_TYPE_NUMPAD, 2, 15);

    swkbdSetHintText(&swkbd, "Porta");

    swkbdSetNumpadKeys(&swkbd, '.', 0);

    SwkbdButton botaoPressionado = swkbdInputText(&swkbd, buffer, sizeof(buffer));

    if (botaoPressionado == SWKBD_BUTTON_RIGHT) {
        return std::string(buffer);
    }

    return "";
}
//FIM TECLADOS

int cenaTexto::update(const InputPacket& packet) {
    auto& botoesAtuais = paginasBotoes[paginaAtual];
    if (packet.keysDown & KEY_DDOWN) {
        botoesAtuais[indiceFoco].selecionado = false;
        indiceFoco = (indiceFoco + 1 + botoesAtuais.size()) % botoesAtuais.size();
        botoesAtuais[indiceFoco].selecionado = true;
    }
    else if (packet.keysDown & KEY_DUP) {
        botoesAtuais[indiceFoco].selecionado = false;
        indiceFoco = (indiceFoco - 1 + botoesAtuais.size()) % botoesAtuais.size();
        botoesAtuais[indiceFoco].selecionado = true;
    }
    else if (packet.keysDown & KEY_DRIGHT) { 
        botoesAtuais[indiceFoco].selecionado = false;
        paginaAtual = 1; 
        indiceFoco = 0;
        paginasBotoes[paginaAtual][indiceFoco].selecionado = true;
    }
    else if (packet.keysDown & KEY_DLEFT) { 
        botoesAtuais[indiceFoco].selecionado = false;
        paginaAtual = 0; 
        indiceFoco = 0;
        paginasBotoes[paginaAtual][indiceFoco].selecionado = true;
    }

    
    else if (packet.keysDown & KEY_A) {
        
        if (paginaAtual == 0) {
            
            if (indiceFoco == 0) {
             std::string novoIP = abrirTecladoIP();
             if (!novoIP.empty()) {
              ipTemp = novoIP;
              std::string novaExibicao = ipTemp + ":" + portaTemp;
              C2D_TextParse(&parametersText, staticBuf, novaExibicao.c_str());
              C2D_TextOptimize(&parametersText);
             }
            }

            else if (indiceFoco == 1) { std::string novaPorta = abrirTecladoPorta();

             if (!novaPorta.empty()) {
                portaTemp = novaPorta;
                std::string novaExibicao = ipTemp + ":" + portaTemp;
                C2D_TextParse(&parametersText, staticBuf, novaExibicao.c_str());
                C2D_TextOptimize(&parametersText); }
            }
            else if (indiceFoco == 2) {
                 ipGlobal = ipTemp;
                porta = portaTemp;
                salvarConfig();
                return 3;
                }
            else if (indiceFoco == 3) {
                 /* algo*/ 
                }
        }
        else if (paginaAtual == 1) {
            if (indiceFoco == 0) {
             //algo
            }
    }
}
    

    if (packet.keysDown & KEY_B) {
        return 3; 
    }

    if (packet.keysHeld & KEY_Y) {
        //aplicar
		//squareColor = C2D_Color32(0, 255, 0, 255);
    }
 
    return -1;
}

void cenaTexto::draw(C3D_RenderTarget* top, C3D_RenderTarget* bottom) {

    float larguraInfo, alturaInfo;
    float  alturaPa;
    
    C2D_TextGetDimensions(&infoText, escalaTexto, escalaTexto, &larguraInfo, &alturaInfo);
    C2D_TextGetDimensions(&parametersText, escalaTexto+0.3f, escalaTexto + 0.3f, &parameterField, &alturaPa);
    float padding = 20.0f;
    float larguraTotalContainer = parameterField + padding;
    float xContainer = getCenter(SCREEN_WIDTH_TOP, larguraTotalContainer);
  
    C2D_SceneBegin(top);
    C2D_TargetClear(top, C2D_Color32(255, 255, 255, 255)); // Fundo Azul

    
    C2D_DrawRectangle(xContainer, 120.0f, 0.8f, larguraTotalContainer, 47.0f, fieldColor, fieldColor, fieldColor, fieldColor);
    C2D_DrawText(&infoText, C2D_AtBaseline | C2D_WithColor, (400.0f-larguraInfo)/2, 110.0f, 0.9f, escalaTexto, escalaTexto, C2D_Color32(0, 0, 0, 255));
    C2D_DrawText(&parametersText, C2D_AtBaseline | C2D_WithColor, (400.0f - parameterField) / 2, 150.0f, 0.9f, escalaTexto+0.3f, escalaTexto + 0.3f, C2D_Color32(0, 0, 0, 255));
    
    header->draw();
   

    C2D_SceneBegin(bottom);
    C2D_TargetClear(bottom, C2D_Color32(20, 20, 20, 255));

   

    rodape->draw();

    for (auto& btn : botoes) {
        btn.draw();
    }

     for (auto& btn : paginasBotoes[paginaAtual]) {
        btn.draw();
    }

     if(paginaAtual == 0){
        C2D_DrawText(&ipText, C2D_WithColor |  C2D_AlignRight , 288, 55.0f, 0.9f, 0.6f, 0.6f, C2D_Color32(0, 0, 0, 255));
        C2D_DrawText(&portaText, C2D_WithColor|  C2D_AlignRight, 288, 95.0f, 0.9f, 0.6f, 0.6f, C2D_Color32(0, 0, 0, 255));
        C2D_DrawText(&deadzoneText, C2D_WithColor|  C2D_AlignRight, 288, 135.0f, 0.9f, 0.6f, 0.6f, C2D_Color32(0, 0, 0, 255));
        C2D_DrawRectangle(270, 174.0f, 1.0f, 22.0f, 22.0f, C2D_Color32(0, 0, 0, 255), C2D_Color32(0, 0, 0, 255), C2D_Color32(0, 0, 0, 255), C2D_Color32(0, 0, 0, 255));
    } else if (paginaAtual == 1){
        C2D_DrawRectangle(270, 134.0f, 1.0f, 22.0f, 22.0f, C2D_Color32(0, 0, 0, 255), C2D_Color32(0, 0, 0, 255), C2D_Color32(0, 0, 0, 255), C2D_Color32(0, 0, 0, 255));
        C2D_DrawRectangle(270, 94.0f, 1.0f, 22.0f, 22.0f, C2D_Color32(0, 0, 0, 255), C2D_Color32(0, 0, 0, 255), C2D_Color32(0, 0, 0, 255), C2D_Color32(0, 0, 0, 255));
        C2D_DrawRectangle(270, 54.0f, 1.0f, 22.0f, 22.0f, C2D_Color32(0, 0, 0, 255), C2D_Color32(0, 0, 0, 255), C2D_Color32(0, 0, 0, 255), C2D_Color32(0, 0, 0, 255));
    }
}