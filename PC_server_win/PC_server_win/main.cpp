#include <iostream>
#include <cstdint>
#include <algorithm>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <ViGEm/Client.h>

// linka a biblioteca de rede do Windows
#pragma comment(lib, "ws2_32.lib")

#pragma pack(push, 1)
struct InputPacket {
    uint32_t keysDown;
    uint32_t keysHeld;
    int16_t circleX, circleY;
    uint16_t touchX, touchY;
};
#pragma pack(pop)

enum {
    BIT_A = 0x1, BIT_B = 0x2, BIT_SELECT = 0x4, BIT_START = 0x8,
    BIT_DRIGHT = 0x10, BIT_DLEFT = 0x20, BIT_DUP = 0x40, BIT_DDOWN = 0x80,
    BIT_R = 0x100, BIT_L = 0x200, BIT_X = 0x400, BIT_Y = 0x800
};

static SHORT Scale3DSToXboxAxis(int16_t v, int16_t srcMin = -150, int16_t srcMax = 150) {
    if (std::abs(v) < 15) return 0;
   
    const float normalized = std::clamp<float>((v - srcMin) / float(srcMax - srcMin), 0.0f, 1.0f) * 2.0f - 1.0f;
    
    if (normalized >= 0.0f) return (SHORT)(normalized * 32767.0f);
    return (SHORT)(normalized * 32768.0f);
}

int main() {
    // 1. INICIALIZAÇÃO DO VIGEM (CONTROLE VIRTUAL)
    auto client = vigem_alloc();
    if (client == nullptr) {
        std::cerr << "Erro: Nao foi possivel alocar memoria para o ViGEm." << std::endl;
        return -1;
    }

    const auto connect_status = vigem_connect(client);
    if (!VIGEM_SUCCESS(connect_status)) {
        std::cerr << "Erro: Driver ViGEmBus nao encontrado. Instale-o primeiro." << std::endl;
        system("pause");
        return -1;
    }

    auto pad = vigem_target_x360_alloc();
    const auto add_status = vigem_target_add(client, pad);
    if (!VIGEM_SUCCESS(add_status)) {
        std::cerr << "Conexao falhou com erro 0x" << std::hex << add_status << std::dec << std::endl;
        vigem_target_free(pad);
        vigem_disconnect(client);
        vigem_free(client);
        system("pause");
        return -1;
    }
    else {
        std::cout << ">> Controle de Xbox 360 Virtual conectado com sucesso!" << std::endl;
    }

    

    // 2. INICIALIZAÇÃO DO WINSOCK (REDE)
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup falhou." << std::endl;
        // limpeza do ViGEm já feito mais acima somente em erro; aqui remove o pad e client
        vigem_target_remove(client, pad);
        vigem_target_free(pad);
        vigem_disconnect(client);
        vigem_free(client);
        system("pause");
        return -1;
    };

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Falha ao criar socket UDP." << std::endl;
        WSACleanup();
        vigem_target_remove(client, pad);
        vigem_target_free(pad);
        vigem_disconnect(client);
        vigem_free(client);
        system("pause");
        return -1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345); // mesma porta do 3DS
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, reinterpret_cast<struct sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Erro ao reservar a porta 12345." << std::endl;
        closesocket(sock);
        WSACleanup();
        vigem_target_remove(client, pad);
        vigem_target_free(pad);
        vigem_disconnect(client);
        vigem_free(client);
        system("pause");
        return -1;
    }

    std::cout << ">> Servidor UDP ouvindo na porta 12345. Aguardando 3DS......" << std::endl;

    
    InputPacket packet;
	sockaddr_in clientAddr{};// endereço do cliente (3DS)?????????????
    


    while (true) {
        int addrLen = sizeof(clientAddr);
        int bytesReceived = recvfrom(sock, reinterpret_cast<char*>(&packet), sizeof(packet), 0,
                                     reinterpret_cast<struct sockaddr*>(&clientAddr), &addrLen);
        if (bytesReceived != sizeof(packet)) {
            // pacote de tamanho inesperado -- ignora
            std::cerr << "Pacote recebido com tamanho inesperado: " << bytesReceived << std::endl;
            continue;
        }

        XUSB_REPORT report;
        XUSB_REPORT_INIT(&report); // garante zeros

        // MAPEAMENTO DE BOTÕES (3DS -> XBOX)
        // Usamos packet.keysHeld para manter o botão pressionado no PC
        if (packet.keysHeld & BIT_A) { report.wButtons |= XUSB_GAMEPAD_A; std::cout << "A pressionado" << std::endl; }
        if (packet.keysHeld & BIT_B) { report.wButtons |= XUSB_GAMEPAD_B; std::cout << "B presionado" << std::endl; }
        if (packet.keysHeld & BIT_X)    report.wButtons |= XUSB_GAMEPAD_X;
        if (packet.keysHeld & BIT_Y)    report.wButtons |= XUSB_GAMEPAD_Y;
        if (packet.keysHeld & BIT_START) report.wButtons |= XUSB_GAMEPAD_START;
        if (packet.keysHeld & BIT_SELECT)report.wButtons |= XUSB_GAMEPAD_BACK;
        if (packet.keysHeld & BIT_DUP)   report.wButtons |= XUSB_GAMEPAD_DPAD_UP;
        if (packet.keysHeld & BIT_DDOWN) report.wButtons |= XUSB_GAMEPAD_DPAD_DOWN;
        if (packet.keysHeld & BIT_DLEFT) report.wButtons |= XUSB_GAMEPAD_DPAD_LEFT;
        if (packet.keysHeld & BIT_DRIGHT)report.wButtons |= XUSB_GAMEPAD_DPAD_RIGHT;
        if (packet.keysHeld & BIT_L)     report.wButtons |= XUSB_GAMEPAD_LEFT_SHOULDER;
        if (packet.keysHeld & BIT_R)     report.wButtons |= XUSB_GAMEPAD_RIGHT_SHOULDER;

        // MAPEAMENTO DOS ANALÓGICOS (Escalonamento Matemático)
        // 3DS (-150 a 150) -> Xbox (-32768 a 32767)
        report.sThumbLX = Scale3DSToXboxAxis(packet.circleX, -150, 150);
        report.sThumbLY = Scale3DSToXboxAxis(packet.circleY, -150, 150);

        // Atualiza o driver com o novo estado
        const auto upd = vigem_target_x360_update(client, pad, report);
        if (!VIGEM_SUCCESS(upd)) {
            std::cerr << "vigem_target_x360_update falhou com erro 0x" << std::hex << upd << std::dec << std::endl;
        }
    }

    // LIMPEZA (não alcançada no loop infinito atual)
    vigem_target_remove(client, pad);
    vigem_target_free(pad);
    vigem_disconnect(client);
    vigem_free(client);

    closesocket(sock);
    WSACleanup();

    return 0;
}