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
bool transmissionPaused = false;
bool isContextPaused = false;
bool swapAB_XY = false;
int deadZone = 15;
bool invertYAxis = false;

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
    bool rodando = true;
	while (aptMainLoop() && rodando) {
		InputPacket packet = input.scan();
		if (packet.keysDown & KEY_START) break;
		//network.sendPacket(packet);

		int nextEstate = cenaAtual? cenaAtual->update(packet): -1;

		if (nextEstate != -1) {
			switch (nextEstate){
				case 10: {
					cenaConectado* cenaC = dynamic_cast<cenaConectado*>(cenaAtual);
				if(cenaC){
					std::vector<uint8_t> matrixData = cenaC->touchpad->extractNormalized(40);

					network.sendMatrix(matrixData);
				}
				break;
				}

				case 11:
				 if (transmissionPaused) {
                 
                 InputPacket ping{};
                 ping.comando = 8;
                  network.sendPacket(ping);
        
                 if (auto cenaC = dynamic_cast<cenaConectado*>(cenaAtual)) {
                   cenaC->setStatus("Testando conexão com o PC...");
                  }   
                  network.resetTimeout(); // Dá mais 3 segundos para o PC responder
                } else {
                   transmissionPaused = true;
                   if (auto cenaC = dynamic_cast<cenaConectado*>(cenaAtual)) {
                   cenaC->setStatus("Transmissão Pausada.");
                   }
                }
                break;

				case 12:{
					cenaConectado* cenaC = dynamic_cast<cenaConectado*>(cenaAtual);
                 if (cenaC) {
                    network.sendText(cenaC->textToDeliver); 
                 }
                 break;
				}

				case 0: {
                bool sucesso = network.connectToServer(ipGlobal, std::stoi(porta));
                if (sucesso) {
                    cenaAtual->setStatus(network.getStatusMessage());
                    estadoConexao = 1;
                } else {
                    cenaAtual->setStatus(info);
                    estadoConexao = 0;
                }
                break;
            }

            case 2:
                delete cenaAtual;
                cenaAtual = nullptr;
                rodando = false; // sinaliza para o while encerrar no próximo frame
                break;

            case 1:
                delete cenaAtual;
				cenaAtual = nullptr;
                cenaAtual = new cenaTexto();
                break;

            case 3:
			if(estadoConexao == 2){
				InputPacket aviso{};
				aviso.comando = 6;
				network.sendPacket(aviso);

			}
                delete cenaAtual;
				cenaAtual = nullptr;
                cenaAtual = new cenaPrincipal();
                estadoConexao = 0;
				transmissionPaused = false;
                break;
                
            default:
                break;
			}

		}

		if (estadoConexao == 1) {
			int status = network.checkConfirmation();

			if (status==1) {
                estadoConexao = 2;
                
				
				delete cenaAtual;
				cenaAtual = new cenaConectado();
				//cenaAtual->setStatus("IP: " + ipGlobal + "\nPORTA: " + porta);

                 
            } else if (status == -1) {
                estadoConexao = 0;
                
                cenaAtual->setStatus(network.getStatusMessage());
            }
			/*
            delete cenaAtual; //essas
			cenaAtual = new cenaConectado();
			estadoConexao = 2; //substituir essas linhas pelo bloco comentado apos o debug

			if (status==1) {
                estadoConexao = 2;
                
				
				delete cenaAtual;
				cenaAtual = new cenaConectado();
				//cenaAtual->setStatus("IP: " + ipGlobal + "\nPORTA: " + porta);

                 
          } else if (status == -1) {
                estadoConexao = 0;
                
                cenaAtual->setStatus(network.getStatusMessage());
            }
			*/
			
            
        }

		if (estadoConexao == 2) {
			int status = network.checkActiveConnection();
			if (status == 2) { 
				//servidor fechou (comando 6)
             delete cenaAtual;
             cenaAtual = new cenaPrincipal();
			 cenaAtual->setStatus("O servidor foi fechado.");
             estadoConexao = 0;
             transmissionPaused = false;
			 
            } else if ( status == -1 && !transmissionPaused){
				transmissionPaused = true;

				if (auto cenaC = dynamic_cast<cenaConectado*>(cenaAtual)) {
                  cenaC->setStatus("PC não responde. Transmissão pausada.");
				}

			} else if(status == 1 && transmissionPaused){
               transmissionPaused = false;
                if (auto cenaC = dynamic_cast<cenaConectado*>(cenaAtual)) {
                 cenaC->setStatus("Conectado e Transmitindo!");
                }
			}
			isContextPaused = false;
			if (auto cenaC = dynamic_cast<cenaConectado*>(cenaAtual)) {
				isContextPaused = cenaC->isBusy();
            }
               if(!transmissionPaused){
				 static int frameCount = 0;
			        if (++frameCount >= 120) {
                     frameCount = 0;
                     InputPacket ping{};
                     ping.comando = 8;
                     network.sendPacket(ping);
                }
			  }
			 

			if (!transmissionPaused &&  !isContextPaused){
              packet.comando = 2;
			  network.sendPacket(packet);

			  
			}
			
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