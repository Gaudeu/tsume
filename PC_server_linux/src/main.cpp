#include <iostream>
#include <cstdint>
#include <algorithm>
#include <cstring>
#include <cmath>
#include <atomic>   
#include <csignal>  

// rede
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h> 

// emular controle
#include <linux/uinput.h>

std::atomic<bool> rodando(true);

void manipulador_sigint(int sinal) {
    if (sinal == SIGINT) {
        rodando = false; 
    }
}

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

static int32_t Scale3DSToLinuxAxis(int16_t v, int16_t srcMin = -150, int16_t srcMax = 150) {
    if (std::abs(v) < 15) return 0;
    const float normalized = std::clamp<float>((v - srcMin) / float(srcMax - srcMin), 0.0f, 1.0f) * 2.0f - 1.0f;
    if (normalized >= 0.0f) return (int32_t)(normalized * 32767.0f);
    return (int32_t)(normalized * 32768.0f);
}

void emit_event(int fd, uint16_t type, uint16_t code, int32_t value) {
    struct input_event ie{};
    ie.type = type;
    ie.code = code;
    ie.value = value;
    write(fd, &ie, sizeof(ie));
}

int main() {
    std::signal(SIGINT, manipulador_sigint);  

    std::cout << "=========================================" << std::endl;
    std::cout << "servidor iniciado!!" << std::endl;
    std::cout << "Pressione Ctrl+C a qualquer momento para sair." << std::endl;
    std::cout << "=========================================" << std::endl;

    int uifd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (uifd < 0) {
        std::cerr << "Erro ao abrir /dev/uinput. Garanta que rodou com sudo!" << std::endl;
        return -1;
    }

    
    ioctl(uifd, UI_SET_EVBIT, EV_KEY);
    ioctl(uifd, UI_SET_KEYBIT, BTN_SOUTH); 
    ioctl(uifd, UI_SET_KEYBIT, BTN_EAST);  
    ioctl(uifd, UI_SET_KEYBIT, BTN_NORTH); 
    ioctl(uifd, UI_SET_KEYBIT, BTN_WEST);  
    ioctl(uifd, UI_SET_KEYBIT, BTN_START);
    ioctl(uifd, UI_SET_KEYBIT, BTN_SELECT);

   
    ioctl(uifd, UI_SET_EVBIT, EV_ABS);
    ioctl(uifd, UI_SET_ABSBIT, ABS_X);
    ioctl(uifd, UI_SET_ABSBIT, ABS_Y);

    
    struct uinput_user_dev uud{};
    std::memset(&uud, 0, sizeof(uud));
    std::strncpy(uud.name, "Microsoft X-Box 360 pad", UINPUT_MAX_NAME_SIZE);
    uud.id.bustype = BUS_USB;
    uud.id.vendor  = 0x045e; // Microsoft
    uud.id.product = 0x028e; // Xbox 360 Controller
    
    
    uud.absmin[ABS_X] = -32768;
    uud.absmax[ABS_X] = 32767;
    uud.absmin[ABS_Y] = -32768;
    uud.absmax[ABS_Y] = 32767;

    
    if (write(uifd, &uud, sizeof(uud)) < 0) {
        std::cerr << "Erro ao escrever configuracao uinput_user_dev" << std::endl;
        close(uifd);
        return -1;
    }

    if (ioctl(uifd, UI_DEV_CREATE) < 0) {
        std::cerr << "Erro ao executar UI_DEV_CREATE" << std::endl;
        close(uifd);
        return -1;
    }

    std::cout << ">> Controle Virtual criado via uinput com sucesso!" << std::endl;

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0) {
        std::cerr << "Falha ao criar socket UDP." << std::endl;
        ioctl(uifd, UI_DEV_DESTROY);
        close(uifd);
        return -1;
    }

    
    fcntl(sock, F_SETFL, O_NONBLOCK);

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(1234);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, reinterpret_cast<struct sockaddr*>(&serverAddr), sizeof(serverAddr)) < 0) {
        std::cerr << "Erro ao reservar a porta 12345." << std::endl;
        close(sock);
        ioctl(uifd, UI_DEV_DESTROY);
        close(uifd);
        return -1;
    }

    std::cout << ">> Servidor UDP ouvindo na porta 12345. Aguardando 3DS..." << std::endl;

    InputPacket packet;
    sockaddr_in clientAddr{};
    socklen_t addrLen = sizeof(clientAddr);

    bool botao_A_estava_pressionado = false; 
    bool botao_B_estava_pressionado = false; 

    while (rodando) {
        uint8_t buffer[1024];

        ssize_t bytesReceived = recvfrom(sock, buffer, sizeof(packet), 0,
                                         reinterpret_cast<struct sockaddr*>(&clientAddr), &addrLen);
        
        if (bytesReceived < 0) {
            usleep(1000); 
            continue;
        }

        if (bytesReceived == sizeof(InputPacket)) {
            
            std::memcpy(&packet, buffer, sizeof(InputPacket));
        } 
        else if (bytesReceived == 784) {
            
            // ignoramos por enquanto
            continue; 
        } 
        else {
            // Pacote quebrado ou desconhecido, apenas ignora
            continue;
        }

        //debug
        bool botao_A_pressionado_agora = (packet.keysHeld & BIT_A);
        if (botao_A_pressionado_agora && !botao_A_estava_pressionado){
            std::cout << "A pressionado" << std::endl;
        }
        bool botao_B_pressionado_agora = (packet.keysHeld & BIT_B);
        if (botao_B_pressionado_agora && !botao_B_estava_pressionado){
            std::cout << "B pressionado" << std::endl;
        }

        botao_A_estava_pressionado = botao_A_pressionado_agora;
        botao_B_estava_pressionado = botao_B_pressionado_agora;

        // MAPEAMENTO DE BOTÕES
        emit_event(uifd, EV_KEY, BTN_SOUTH, (packet.keysHeld & BIT_A) ? 1 : 0);
        emit_event(uifd, EV_KEY, BTN_EAST,  (packet.keysHeld & BIT_B) ? 1 : 0);
        emit_event(uifd, EV_KEY, BTN_NORTH, (packet.keysHeld & BIT_X) ? 1 : 0);
        emit_event(uifd, EV_KEY, BTN_WEST,  (packet.keysHeld & BIT_Y) ? 1 : 0);
        emit_event(uifd, EV_KEY, BTN_START, (packet.keysHeld & BIT_START) ? 1 : 0);
        emit_event(uifd, EV_KEY, BTN_SELECT,(packet.keysHeld & BIT_SELECT) ? 1 : 0);

        // MAPEAMENTO DOS ANALÓGICOS 
        int32_t lx = Scale3DSToLinuxAxis(packet.circleX);
        int32_t ly = -Scale3DSToLinuxAxis(packet.circleY); 
        
        emit_event(uifd, EV_ABS, ABS_X, lx);
        emit_event(uifd, EV_ABS, ABS_Y, ly);

        emit_event(uifd, EV_SYN, SYN_REPORT, 0);
    }

    std::cout << "\n=======================" << std::endl;
    std::cout << "encerrando programa, até!!!!!" << std::endl;
    std::cout << "\n=======================" << std::endl;

    ioctl(uifd, UI_DEV_DESTROY);
    close(uifd);
    close(sock);
    return 0;
}