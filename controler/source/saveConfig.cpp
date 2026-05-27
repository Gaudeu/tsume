#include "saveConfig.h"
#include "Common.h" // Necessário para acessar ipGlobal e porta
#include <fstream>
#include <string>

const std::string pathConfig = "sdmc:/3ds/config_controle.txt";

void salvarConfig() {
    std::ofstream arquivo(pathConfig);
    if (arquivo.is_open()) {
        arquivo << ipGlobal << "\n";
        arquivo << porta << "\n";
        arquivo.close();
    }
}

void carregarConfig() {
    std::ifstream arquivo(pathConfig);
    if (arquivo.is_open()) {
        std::getline(arquivo, ipGlobal);
        std::getline(arquivo, porta);
        arquivo.close();
    }
}