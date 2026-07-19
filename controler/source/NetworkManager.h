#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <string>
#include <arpa/inet.h>
#include "Common.h"
#include <vector>

#pragma pack(push, 1)
struct TextPacket {
    uint8_t comando;  
    char text[255];  
};
#pragma pack(pop)

class NetworkManager {
private:
    int sock;
    struct sockaddr_in server_addr;
    uint32_t* soc_buffer; // buffer necesserio para o socInit do 3DS 
    std::string statusMessage; // vai guardar erros ou sucesso
    bool isConnected;
    int timeoutCounter = 0;
    int tempoEspera = 0;

public:
    NetworkManager();
    ~NetworkManager(); 

    bool connectToServer(const std::string& ip, int port);
    void pauseConnection();
    void resetTimeout() { timeoutCounter = 0; }
    int checkActiveConnection();
    void disconnect();
    int checkConfirmation();
    void sendPacket(const InputPacket& packet); 
    void sendMatrix(const std::vector<uint8_t>& matrix);
    void sendText(const std::string& text);
    std::string getStatusMessage() const { return statusMessage; }
    //bool getIsConnected() const { return isConnected; }
};

#endif