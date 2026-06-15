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

    isConnected = true;
    statusMessage = "Rede pronta! Mirando em " + ip + ":" + std::to_string(port);
    return true;

}


void NetworkManager::sendMatrix(const std::vector<uint8_t>& matrix) {
    if (!isConnected || sock < 0 || matrix.empty()) return;
    
    // Envia o array de bytes pela rede (784 bytes)
    sendto(sock, matrix.data(), matrix.size(), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
}

void NetworkManager::sendPacket(const InputPacket& packet) {

    
	 
     
    if (!isConnected || sock < 0) { return; }

    /*revisao
    if (memcmp(&packet, &lastPacket, sizeof(InputPacket)) == 0) {
        return; 
    }*/
     //sockfd, buf, len, flags, dest_addr, addrlen
    //int sent = sendto(sock, &packet, sizeof(packet), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
    sendto(sock, &packet, sizeof(packet), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
    lastPacket = packet;
}