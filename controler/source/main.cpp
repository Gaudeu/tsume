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

std::string ipGlobal = "127.0.0.1"; // Definição real
std::string porta = "12345";
int estadoConexao = 0;//0: desconectado, 1:aguardando confirmacao, 2: conectado


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

		int nextEstate = cenaAtual? cenaAtual->update(packet): -1;

		if (nextEstate != -1) {
			Cena* proximaCena = nullptr;

			if (nextEstate == 10){
				//enviar a matriz
				cenaConectado* cenaC = dynamic_cast<cenaConectado*>(cenaAtual);
				if(cenaC){
					std::vector<uint8_t> matrixData = cenaC->touchpad->extractNormalized(40);

					network.sendMatrix(matrixData);
				}
					nextEstate = -1;

			} else if (nextEstate == 11) {
				//network.pauseConnection()
			} else if (nextEstate == 0) {
				
				bool sucesso = network.connectToServer(ipGlobal, std::stoi(porta));//true = socket conectado ou false = erro
				if(sucesso){
                  cenaAtual->setStatus(network.getStatusMessage());
				  estadoConexao = 1;
                  
				} else {
                  cenaAtual->setStatus(info);
				  estadoConexao = 0;
                  
				}
				
			}
			else {
				
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

		if (estadoConexao == 1) {
			int status = network.checkConfirmation();
            if (status==1) {
                estadoConexao = 2;
                
				
				delete cenaAtual;
				cenaAtual = new cenaConectado();
				cenaAtual->setStatus("IP: " + ipGlobal + "\nPORTA: " + porta);

                 
          } else if (status == -1) {
                estadoConexao = 0;
                
                cenaAtual->setStatus(network.getStatusMessage());
            }
        }

		if (estadoConexao == 2) {
			packet.comando = 2;
			network.sendPacket(packet);
		}

		// Desenha a imagem se ela existir
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		if (cenaAtual != nullptr) {
            cenaAtual->draw(top, bottom);
         } else {
			C2D_TargetClear(top, C2D_Color32(0, 0, 0, 255));
            C2D_TargetClear(bottom, C2D_Color32(0, 0, 0, 255));
		 } 
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