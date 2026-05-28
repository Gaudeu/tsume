#include<stdio.h>
#include <citro2d.h>
#include <citro3d.h>
#include "InputHandler.h"
#include "NetworkManager.h"
#include "cenaTexto.h"
#include "cenaPrincipal.h"
#include "cenaConectado.h"
#include "Common.h"
#include "saveConfig.h"

std::string ipGlobal = "192.168.0.10"; // Definição real
std::string porta = "1234";
bool conectado = false;


#define SCREEN_WIDTH 400
#define SCREEN_HEIGHT 240

int main() {
	carregarConfig();
	gfxInitDefault();                        //liga a tela no modo padrao
	ptmuInit();

	std::string info = "info";

	Result rc = romfsInit();
	if (R_FAILED(rc)) {
		
		info = "Erro ao iniciar RomFS";
	}
	else {
		info ="RomFS iniciado com sucesso.\n";
	}                            

	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);       
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();                           //prepara a gpu para receber comandos de desenho

	NetworkManager network;
	InputHandler input;
	//alvo
	C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	C3D_RenderTarget* bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
	
	Cena* cenaAtual = new cenaPrincipal();

	cenaAtual->setStatus(info);

	while (aptMainLoop()) {
		InputPacket packet = input.scan();
		if (packet.keysDown & KEY_START) break;
		//network.sendPacket(packet);

		int nextEstate = cenaAtual->update(packet);

		if (nextEstate != -1) {
			Cena* proximaCena = nullptr;

			if (nextEstate == 10){
				//enviar a matriz
					nextEstate = -1;
			} else if (nextEstate == 0) {
				delete cenaAtual;
				cenaAtual = new cenaConectado();
				bool sucesso = network.connectToServer(ipGlobal, std::stoi(porta));//true = socket conectado ou false = erro
				info = network.getStatusMessage();
				cenaAtual->setStatus(info);
				conectado = true;
			}
			else {
				conectado = false;
				delete cenaAtual;
				cenaAtual = nullptr;
				if (nextEstate == 2) { break; };

				if (nextEstate == 1) {
					cenaAtual = new cenaTexto();
				}
				else if (nextEstate == 3) { // Supondo que 3 volta da cenaTexto
					cenaAtual = new cenaPrincipal();
				}

			}

		}
		if (conectado) {
			network.sendPacket(packet);
		}

		// Desenha a imagem se ela existir
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		cenaAtual->draw(top, bottom);
		C3D_FrameEnd(0);
		// O terminal na tela de baixo é atualizado automaticamente pelo consoleInit
	}

	// Limpeza
	
	delete cenaAtual;
	C2D_Fini();
	C3D_Fini();
	ptmuExit();
	romfsExit();
	gfxExit();

	return 0;
}