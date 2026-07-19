//arrumar uma maneira de quando eu conecto pelo 3ds ele envia um pedido
//de conexao e aguarda o pc confirmar para entao comecar a transmitir os packets
#include "NetworkManager.h"
#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>

#include <3ds.h>

NetworkManager::NetworkManager() {
    sock = -1;
    soc_buffer = nullptr;
    isConnected = false;
    statusMessage = "Aguardando conexão...";
   
}

NetworkManager::~NetworkManager() {                 //destructor
    disconnect();
}
/*
void NetworkManager::pauseConnection(){
    
}
*/


void NetworkManager::disconnect() {
    if (sock >= 0) {
        close(sock);
        sock = -1;
    }
    if (soc_buffer) {
        socExit();
        free(soc_buffer);
        soc_buffer = nullptr;
    }
    isConnected = false;
    statusMessage = "Desconectado.";
}

bool NetworkManager::connectToServer(const std::string& ip, int port) {
    disconnect();
    soc_buffer = (uint32_t*)memalign(0x1000, 0x100000);
    if (!soc_buffer) {
        statusMessage = "Erro: sem memoria para soc_buffer - sample";
        return false;
    }

    Result ret = socInit(soc_buffer, 0x100000);
    if (R_FAILED(ret)) {
        statusMessage = "Erro no socInit. O WiFi do 3DS esta ligado? - Sample";
        free(soc_buffer);
        soc_buffer = nullptr;
        return false;
    }

    //criar socket
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        statusMessage = "Erro: Falha ao criar o socket UDP";
        socExit();
        free(soc_buffer);
        soc_buffer = nullptr;
        return false;
    }
    //revisao
    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);
    //
    //configura o alvo(revisar)
    memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

    //solicitacao
    InputPacket solicitacao{};
    solicitacao.comando = 0;
    sendto(sock, &solicitacao, sizeof(solicitacao), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));

    isConnected = false;
    tempoEspera = 0;
    statusMessage = "Aguardando confirmação do servidor...";

    //isConnected = true;
    //statusMessage = "Rede pronta! Mirando em " + ip + ":" + std::to_string(port);
    return true;

}

int NetworkManager::checkConfirmation(){
    tempoEspera++;
    if (tempoEspera > 900) {
        isConnected = false;
        statusMessage = "Conexao falhou (Tempo Esgotado). Tente novamente.";
        return -1; // Retornamos -1 para indicar Timeout/Falha
    }

    InputPacket pacoteRecebido{};
    struct sockaddr_in fromAddr;
    socklen_t fromLen = sizeof(fromAddr);

    ssize_t bytes = recvfrom(sock, &pacoteRecebido, sizeof(pacoteRecebido), 0, (struct sockaddr*)&fromAddr, &fromLen);

    if (bytes==sizeof(InputPacket)) {
        if (pacoteRecebido.comando == 1) { // 1 = PC disse SIM
            isConnected = true;
            statusMessage = "Conectado com sucesso!";
            return 1;
        } else if (pacoteRecebido.comando == 4) {
            
            isConnected = false;
            statusMessage = "Conexao recusada pelo servidor.";
            return -1; 
        } else if (pacoteRecebido.comando == 6){
            statusMessage = "O servidor foi fechado.";
            isConnected = false;

            return 3;

        }
    }
    return 0;
}

int NetworkManager::checkActiveConnection() {
    InputPacket pacoteRecebido{};
    struct sockaddr_in fromAddr;
    socklen_t fromLen = sizeof(fromAddr);

    int statusRetorno = 0;

    while (recvfrom(sock, &pacoteRecebido, sizeof(pacoteRecebido), 0, (struct sockaddr*)&fromAddr, &fromLen) == sizeof(InputPacket)) {
        
        if (pacoteRecebido.comando == 6) { // servidor fechou definitivamente
            //mensagem de "servidor fechou"
            disconnect();
            return 2; 
        }
        if (pacoteRecebido.comando == 9) { // PONG recebido
            timeoutCounter = 0; // 
            return 1; 
        }
    }

    
    timeoutCounter++;
    if (timeoutCounter > 360) { 
        return -1; // Soft Pause
    }

    return 0; // Tudo normaR
}


void NetworkManager::sendMatrix(const std::vector<uint8_t>& matrix) {
    if (!isConnected || sock < 0 || matrix.empty()) return;
    
    // Envia o array de bytes pela rede (784 bytes)
    sendto(sock, matrix.data(), matrix.size(), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
}

void NetworkManager::sendText(const std::string& text){
    if (!isConnected || sock < 0 || text.empty()) return;

    TextPacket packet{};
    packet.comando = 5;

    memset(packet.text, 0, sizeof(packet.text));

    strncpy(packet.text, text.c_str(), sizeof(packet.text) - 1);

    sendto(sock, &packet, sizeof(packet), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));

}

void NetworkManager::sendPacket(const InputPacket& packet) {

    
	 
     
    if (!isConnected || sock < 0) { return; }

    sendto(sock, &packet, sizeof(packet), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
}