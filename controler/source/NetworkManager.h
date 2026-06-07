#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <string>
#include <arpa/inet.h>
#include "Common.h"
#include <vector>

class NetworkManager {
private:
    int sock;
    struct sockaddr_in server_addr;
    uint32_t* soc_buffer; // buffer necesserio para o socInit do 3DS 
    std::string statusMessage; // vai guardar nossos erros ou sucesso
    InputPacket lastPacket;
    bool isConnected;

public:
    NetworkManager();
    ~NetworkManager(); 

    bool connectToServer(const std::string& ip, int port);
    void disconnect();

    void sendPacket(const InputPacket& packet); // M�todo de envio
    void sendMatrix(const std::vector<uint8_t>& matrix);
    std::string getStatusMessage() const { return statusMessage; }
    bool getIsConnected() const { return isConnected; }
};

#endif