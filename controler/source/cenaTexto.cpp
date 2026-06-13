#include "cenaTexto.h"
#include "saveConfig.h"
const char* altIP = "alterar IP";
const char* altPort = "alterar Porta";
const char* OKstring = "Ok";
bool ok = false;
float parameterField = 190.0f;
float xBtnBot = getCenter(SCREEN_WIDTH_BOTTOM, BTN_WIDTH);
//float xBtnTop = getCenter(SCREEN_WIDTH_TOP, parameterField);
float escalaTexto = 0.8f;
cenaTexto::cenaTexto() {
    header = new PainelTopo(25.0f);
    rodape = new PainelRodape();

    staticBuf = C2D_TextBufNew(4096);

    ipTemp = ipGlobal;
    portaTemp = porta;

    C2D_TextParse(&infoText, staticBuf, "[IP:PORT]");
    std::string displayParam = ipTemp + ":" + portaTemp;
    C2D_TextParse(&parametersText, staticBuf, displayParam.c_str());
    C2D_TextParse(&botaoTextIP, staticBuf, altIP);
    C2D_TextParse(&botaoTextPort, staticBuf, altPort);
    C2D_TextParse(&botaoConfirm, staticBuf, OKstring);
    C2D_TextOptimize(&botaoConfirm);
    C2D_TextOptimize(&botaoTextIP);
    C2D_TextOptimize(&botaoTextPort);
    C2D_TextOptimize(&infoText);

    //conteudo botoes
    conteudoBotao* txtBtnIP = new ConteudoTexto(&botaoTextIP);
    conteudoBotao* txtBtnPORT = new ConteudoTexto(&botaoTextPort);
    conteudoBotao* txtBtnConfirm = new ConteudoTexto(&botaoConfirm);


	squareColor = C2D_Color32(255, 255, 255, 255); 
    fieldColor = C2D_Color32(240, 240, 240, 255);
    float centroX = (SCREEN_WIDTH_BOTTOM / 2.0f) - (BTN_WIDTH / 2.0f);
    float centroY = (SCREEN_HEIGHT / 2.0f) - (BTN_HEIGHT / 2.0f);
    
    botoes.push_back(Botao(xBtnBot, 
        centroY - 50, 
        0.5f, 
        BTN_WIDTH, 
        BTN_HEIGHT, 
        txtBtnIP, 
        squareColor, 
        C2D_Color32(200, 200, 200, 255)));
    botoes.push_back(Botao(xBtnBot, 
        centroY - 10, 
        0.5f, 
        BTN_WIDTH, 
        BTN_HEIGHT, 
        txtBtnPORT, 
        squareColor, 
        C2D_Color32(200, 200, 200, 255)));
    botoes.push_back(Botao(xBtnBot, 
        centroY + 30, 
        0.5f, 
        BTN_WIDTH, 
        BTN_HEIGHT, 
        txtBtnConfirm, 
        squareColor, 
        C2D_Color32(200, 200, 200, 255)));
    botoes[0].selecionado = true;
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
    if (packet.keysDown & KEY_DDOWN) {
        botoes[indiceFoco].selecionado = false;
        indiceFoco = (indiceFoco + 1 + botoes.size()) % botoes.size();
        botoes[indiceFoco].selecionado = true;
    }
    else if (packet.keysDown & KEY_A ) {
        if (indiceFoco==0){
        std::string novoIP = abrirTecladoIP();
          if (!novoIP.empty()) {
            ipTemp = novoIP;
            std::string novaExibicao = ipTemp + ":" + portaTemp;
            C2D_TextParse(&parametersText, staticBuf, novaExibicao.c_str());
            C2D_TextOptimize(&parametersText);
          }
        }
        else if (indiceFoco == 1) {
            std::string novaPorta = abrirTecladoPorta();

            if (!novaPorta.empty()) {
                portaTemp = novaPorta;
                std::string novaExibicao = ipTemp + ":" + portaTemp;
                C2D_TextParse(&parametersText, staticBuf, novaExibicao.c_str());
                C2D_TextOptimize(&parametersText);
            }
        }
        else if (indiceFoco == 2) { 
            ipGlobal = ipTemp;
            porta = portaTemp;

            salvarConfig();
            return 3;
        }
    }
    
    else if (packet.keysDown & KEY_DUP) {
        botoes[indiceFoco].selecionado = false;
        indiceFoco = (indiceFoco - 1 + botoes.size()) % botoes.size();
        botoes[indiceFoco].selecionado = true;
    }

    if (packet.keysDown & KEY_B) {
        return 3; // Qualquer valor diferente de 0 e 1 faz voltar � Principal
    }

    if (packet.keysHeld & KEY_X) {
		squareColor = C2D_Color32(0, 255, 0, 255);
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
    C2D_TargetClear(bottom, C2D_Color32(0, 0, 0, 255));

    rodape->draw();

     for (auto& btn : botoes) {
        btn.draw();
    }
}