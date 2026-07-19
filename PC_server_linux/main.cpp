#include <iostream>
#include <cstdint>
#include <algorithm>
#include <cstring>
#include <cmath>
#include <atomic>   
#include <csignal>  
#include <cctype>

// rede
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h> 
#include <arpa/inet.h>

// emular controle
#include <linux/uinput.h>

std::atomic<bool> rodando(true);

int porta = 12345;
int sockGlobal = -1;
sockaddr_in clientAddrGlobal{};
socklen_t addrLenGlobal = sizeof(clientAddrGlobal);
bool clienteConectado = false;

#pragma pack(push, 1)
struct InputPacket {
    uint8_t comando;
    uint32_t keysUp;
    uint32_t keysDown;
    uint32_t keysHeld;
    int16_t circleX, circleY;
    uint16_t touchX, touchY;
};
#pragma pack(pop)

void manipulador_sigint(int sinal) { //tambem aceita SIGTERM
    if (sinal == SIGINT || sinal == SIGTERM && sockGlobal != -1) {
        if(clienteConectado){
           InputPacket avisoMorte{};
           avisoMorte.comando = 6;
           sendto(sockGlobal, &avisoMorte, sizeof(avisoMorte), 0, 
                  (struct sockaddr*)&clientAddrGlobal, addrLenGlobal);
        }
        

        rodando = false; 
    }
}


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

int ascii_to_keycode(char c, bool &shift) {
    shift = false;
    
    
    if (std::isupper(c)) {
        shift = true;
        c = std::tolower(c);
    }
    
    switch (c) {
        case 'a': return KEY_A; case 'b': return KEY_B; case 'c': return KEY_C;
        case 'd': return KEY_D; case 'e': return KEY_E; case 'f': return KEY_F;
        case 'g': return KEY_G; case 'h': return KEY_H; case 'i': return KEY_I;
        case 'j': return KEY_J; case 'k': return KEY_K; case 'l': return KEY_L;
        case 'm': return KEY_M; case 'n': return KEY_N; case 'o': return KEY_O;
        case 'p': return KEY_P; case 'q': return KEY_Q; case 'r': return KEY_R;
        case 's': return KEY_S; case 't': return KEY_T; case 'u': return KEY_U;
        case 'v': return KEY_V; case 'w': return KEY_W; case 'x': return KEY_X;
        case 'y': return KEY_Y; case 'z': return KEY_Z;
        case '1': return KEY_1; case '2': return KEY_2; case '3': return KEY_3;
        case '4': return KEY_4; case '5': return KEY_5; case '6': return KEY_6;
        case '7': return KEY_7; case '8': return KEY_8; case '9': return KEY_9;
        case '0': return KEY_0;
        case ' ': return KEY_SPACE;
        case '\n': return KEY_ENTER;
        case '\b': return KEY_BACKSPACE;
        default: return 0; // ignora caracteres não mapeados
    }
}

void type_string(int uifd, const char* str) {
    for (size_t i = 0; i < std::strlen(str); ++i) {
        bool shift;
        int keycode = ascii_to_keycode(str[i], shift);
        
        if (keycode == 0) continue; // pula o que não souber digitar

        // shift se necessario
        if (shift) {
            emit_event(uifd, EV_KEY, KEY_LEFTSHIFT, 1);
            emit_event(uifd, EV_SYN, SYN_REPORT, 0);
        }

        // Pressiona e solta a tecla
        emit_event(uifd, EV_KEY, keycode, 1);
        emit_event(uifd, EV_SYN, SYN_REPORT, 0);
        
        emit_event(uifd, EV_KEY, keycode, 0);
        emit_event(uifd, EV_SYN, SYN_REPORT, 0);

        // Solta o Shift se foi usado
        if (shift) {
            emit_event(uifd, EV_KEY, KEY_LEFTSHIFT, 0);
            emit_event(uifd, EV_SYN, SYN_REPORT, 0);
        }
    }
}

int main(int argc, char* argv[]) {
    setvbuf(stdout, NULL, _IONBF, 0);

    if (argc > 1) {
        porta = std::atoi(argv[1]);
    }

    int to_right = 0;
    if (argc>2){
        to_right = std::atoi(argv[2]);
    }

    std::signal(SIGINT, manipulador_sigint);  
    std::signal(SIGTERM, manipulador_sigint);

    std::cout << "=========================================" << std::endl;
    std::cout << "servidor initialized" << std::endl;
    std::cout << "=========================================" << std::endl;

    int uifd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (uifd < 0) {
        std::cerr << "Erro ao abrir /dev/uinput." << std::endl;
        return -1;
    }

    
    ioctl(uifd, UI_SET_EVBIT, EV_KEY);
    ioctl(uifd, UI_SET_KEYBIT, BTN_SOUTH); 
    ioctl(uifd, UI_SET_KEYBIT, BTN_EAST);  
    ioctl(uifd, UI_SET_KEYBIT, BTN_NORTH); 
    ioctl(uifd, UI_SET_KEYBIT, BTN_WEST);  
    ioctl(uifd, UI_SET_KEYBIT, BTN_TR); 
    ioctl(uifd, UI_SET_KEYBIT, BTN_TL); 
    ioctl(uifd, UI_SET_KEYBIT, BTN_DPAD_DOWN);
    ioctl(uifd, UI_SET_KEYBIT, BTN_DPAD_UP);
    ioctl(uifd, UI_SET_KEYBIT, BTN_DPAD_LEFT);
    ioctl(uifd, UI_SET_KEYBIT, BTN_DPAD_RIGHT);
    ioctl(uifd, UI_SET_KEYBIT, BTN_START);
    ioctl(uifd, UI_SET_KEYBIT, BTN_SELECT);

   
    ioctl(uifd, UI_SET_EVBIT, EV_ABS);
    ioctl(uifd, UI_SET_ABSBIT, ABS_X);
    ioctl(uifd, UI_SET_ABSBIT, ABS_Y);
    ioctl(uifd, UI_SET_ABSBIT, ABS_RX); 
    ioctl(uifd, UI_SET_ABSBIT, ABS_RY);

    for (int i = KEY_ESC; i <= KEY_SPACE; i++) {
        ioctl(uifd, UI_SET_KEYBIT, i);
    }
    ioctl(uifd, UI_SET_KEYBIT, KEY_LEFTSHIFT); // Para permitir maiúsculas
    ioctl(uifd, UI_SET_KEYBIT, KEY_BACKSPACE);
    ioctl(uifd, UI_SET_KEYBIT, KEY_ENTER);

    
    struct uinput_user_dev uud{};
    std::memset(&uud, 0, sizeof(uud));
    std::strncpy(uud.name, "Microsoft X-Box 360 pad", UINPUT_MAX_NAME_SIZE);
    uud.id.bustype = BUS_USB;
    uud.id.vendor  = 0x045e; // Microsoft
    uud.id.product = 0x028e; // Xbox 360 Controller
    
    
    uud.absmin[ABS_X] = -32768; uud.absmax[ABS_X] = 32767;
    uud.absmin[ABS_Y] = -32768; uud.absmax[ABS_Y] = 32767;
    uud.absmin[ABS_RX] = -32768; uud.absmax[ABS_RX] = 32767;
    uud.absmin[ABS_RY] = -32768; uud.absmax[ABS_RY] = 32767;

    
    if (write(uifd, &uud, sizeof(uud)) < 0) {
        std::cerr << "Error on writing in uinput_user_dev" << std::endl;
        close(uifd);
        return -1;
    }

    if (ioctl(uifd, UI_DEV_CREATE) < 0) {
        std::cerr << "Erro ao executar UI_DEV_CREATE" << std::endl;
        close(uifd);
        return -1;
    }

    std::cout << ">> Virtual controller created sucessfully" << std::endl;

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
    serverAddr.sin_port = htons(porta);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, reinterpret_cast<struct sockaddr*>(&serverAddr), sizeof(serverAddr)) < 0) {
        std::cerr << "Erro ao reservar a porta." << std::endl;
        close(sock);
        ioctl(uifd, UI_DEV_DESTROY);
        close(uifd);
        return -1;
    }

    std::cout << ">> UDP SERVER listening on PORT "<< porta <<". waiting for 3DS..." << std::endl;

    InputPacket packet;
    sockaddr_in clientAddr{};
    socklen_t addrLen = sizeof(clientAddr);

    bool botao_A_estava_pressionado = false; 
    bool botao_B_estava_pressionado = false; 

    while (rodando) {
        uint8_t buffer[1024];

        ssize_t bytesReceived = recvfrom(sock, buffer, sizeof(buffer), 0,
                                         reinterpret_cast<struct sockaddr*>(&clientAddr), &addrLen);
        
     if (bytesReceived < 0) {
            usleep(1000); 
            continue;
        }

        if (buffer[0] == 5) {
            char text[256];
            std::memset(text, 0, sizeof(text));
            
            
            int text_syze = std::min((int)bytesReceived - 1, 255);
            std::memcpy(text, buffer + 1, text_syze);
            
            std::cout << "[NETWORK] Digitando texto: " << text << std::endl;
            type_string(uifd, text);
            continue; 
        }

        if (bytesReceived == sizeof(InputPacket)) {
            std::memcpy(&packet, buffer, sizeof(InputPacket));

            if (packet.comando == 8) {
             InputPacket pong{};
             pong.comando = 9;
             sendto(sock, &pong, sizeof(pong), 0, reinterpret_cast<struct sockaddr*>(&clientAddr), addrLen);
             continue; // Já tratamos, vai para o proximo
    }
        } 
        else if (bytesReceived == 200) {
            std::cout << "[NETWORK] Matriz recebida " << std::endl;
            // matriz... 
            continue; 
        } 
        else {
            // pacote quebrado ou desconhecido, apenas ignora
            continue;
        }

     if(packet.comando == 0){
            std::cout << "[NETWORK] request received FROM: " << inet_ntoa(clientAddr.sin_addr) << std::endl;

            char resposta;
            std::cin >> resposta;

            InputPacket respostaPacote{};

            if (resposta == 'y' || resposta == 'Y') {
             respostaPacote.comando = 1; // 1 = SIM

             sockGlobal = sock;
             clientAddrGlobal = clientAddr;
             clienteConectado = true;

             std::cout << "Connection established! initializing controls..." << std::endl;
             clientAddrGlobal = clientAddr;

            } else {
             respostaPacote.comando = 4; // 4 = NÃO
             std::cout << "Connection denied." << std::endl;
            }

           sendto(sock, &respostaPacote, sizeof(respostaPacote), 0, (struct sockaddr*)&clientAddr, addrLen);

         continue;

        }  else if(packet.comando ==6){
                  std::cout << "[NETWORK] O 3DS se desconectou." << std::endl;
                 continue;
        }  else if(packet.comando == 2){                       
        

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
        emit_event(uifd, EV_KEY, BTN_TR,  (packet.keysHeld & BIT_R) ? 1 : 0);
        emit_event(uifd, EV_KEY, BTN_TL,  (packet.keysHeld & BIT_L) ? 1 : 0);
        emit_event(uifd, EV_KEY, BTN_START, (packet.keysHeld & BIT_START) ? 1 : 0);
        emit_event(uifd, EV_KEY, BTN_SELECT,(packet.keysHeld & BIT_SELECT) ? 1 : 0);
        emit_event(uifd, EV_KEY, BTN_DPAD_DOWN,(packet.keysHeld & BIT_DDOWN) ? 1 : 0);
        emit_event(uifd, EV_KEY, BTN_DPAD_UP,(packet.keysHeld & BIT_DUP) ? 1 : 0);
        emit_event(uifd, EV_KEY, BTN_DPAD_LEFT,(packet.keysHeld & BIT_DLEFT) ? 1 : 0);
        emit_event(uifd, EV_KEY, BTN_DPAD_RIGHT,(packet.keysHeld & BIT_DRIGHT) ? 1 : 0);

        // MAPEAMENTO DOS ANALÓGICOS 
        int32_t lx = Scale3DSToLinuxAxis(packet.circleX);
        int32_t ly = -Scale3DSToLinuxAxis(packet.circleY); 
        
        // verifica qual analogico foi escolhido
        if (to_right == 1) {
            emit_event(uifd, EV_ABS, ABS_RX, lx);
            emit_event(uifd, EV_ABS, ABS_RY, ly);
        } else {
            emit_event(uifd, EV_ABS, ABS_X, lx);
            emit_event(uifd, EV_ABS, ABS_Y, ly);
        }

        emit_event(uifd, EV_SYN, SYN_REPORT, 0);
        }
    }

    std::cout << "\n=======================" << std::endl;
    std::cout << "shutting down..." << std::endl;
    std::cout << "\n=======================" << std::endl;

    ioctl(uifd, UI_DEV_DESTROY);
    close(uifd);
    close(sock);
    return 0;
}
