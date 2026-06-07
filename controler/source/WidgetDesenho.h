#pragma once
#include <Widget.h>
#include <vector>
#include <cmath>
#include <algorithm>



class WidgetDesenho: public Widget 
{
 private:
  std::vector<Ponto2D> rastro;
  std::vector<Ponto2D> rastroCopy;
  u32 corLinha;
  float espessuraLinha;

  //time vars
  bool writing = false;
  int inativity = 0;
  const int LIMIT_FRAME_TIME = 180;

 public:
  WidgetDesenho(float x, float y, float z, float w, float h, u32 corFundo, u32 corLinha)
    : Widget(x, y, z, w, h, corFundo, corFundo, 0.0f, false), corLinha(corLinha), espessuraLinha(3.0f) {}
  
  bool contemPonto(float tx, float ty){
     return(tx >= x && tx <= x + w && ty >= y && ty <= y + h);
  }

  bool update(const InputPacket& packet) {

    if (packet.keysHeld & KEY_TOUCH){
        if (contemPonto(packet.touchX, packet.touchY)){

            bool newTrace = rastro.empty() || (packet.keysDown & KEY_TOUCH);
            Ponto2D novoPonto = { (float)packet.touchX, (float)packet.touchY, newTrace};
            writing = true;
            inativity = 0;
            if(newTrace){
                rastro.push_back(novoPonto);
            } else {
                Ponto2D ultimo = rastro.back();
                 float dist = std::sqrt(std::pow(novoPonto.x - ultimo.x, 2) + std::pow(novoPonto.y - ultimo.y, 2));
                 if(dist > 2.0f){
                    rastro.push_back(novoPonto);
                }
            }
        }
    }

    if ((packet.keysUp & KEY_TOUCH) && !rastro.empty()){
        writing = false;
    }

    if(!writing && !rastro.empty()){
        inativity++;

        if(inativity >= LIMIT_FRAME_TIME){
            rastroCopy = rastro;
            limpar();
            return true;
        }

    }
    return false;
  }

  //tratamento do desenho
 std::vector<uint8_t> extractNormalized(int res = 28){
    std::vector<uint8_t> matrix(res * res, 0);

    if(rastroCopy.empty()) return matrix;

    float minX = 9999.0f, maxX = -9999.0f;
    float minY = 9999.0f, maxY = -9999.0f;

    for (const auto& p: rastroCopy){
        if(p.x < minX) minX = p.x;
        if(p.x > maxX) maxX = p.x;
        if(p.y < minY) minY = p.y;
        if(p.y > maxY) maxY = p.y;
    }

    //bounding box
    float width = maxX - minX;
    float height = maxY - minY;
    float biggest = std::max(width, height);

    if(biggest < 1.0f) biggest = 1.0f;

    float offsetX = minX - (biggest - width)/2.0f;
    float offsetY = minY - (biggest - height)/2.0f;

    float scalingFactor = (res-4)/biggest;
    float margin = 2.0f;

    for(size_t i = 1; i<rastroCopy.size(); i++){
        if(rastroCopy[i].iniciaNovoTraco) continue;
        int x1 = (int)((rastroCopy[i-1].x - offsetX) * scalingFactor + margin);
        int y1 = (int)((rastroCopy[i-1].y - offsetY) * scalingFactor + margin);
        int x2 = (int)((rastroCopy[i].x - offsetX) * scalingFactor + margin);
        int y2 = (int)((rastroCopy[i].y - offsetY) * scalingFactor + margin);

        int steps = std::max(std::abs(x2 - x1), std::abs(y2 - y1));
        if(steps == 0) steps = 1;

        for (int p = 0; p <= steps; p++) {
            float t = (float)p / steps;
            int xPixel = x1 + t * (x2 - x1);
            int yPixel = y1 + t * (y2 - y1);

            // garante que nao vai acessar memoria ilegal
            if (xPixel >= 0 && xPixel < res && yPixel >= 0 && yPixel < res) {
                matrix[yPixel * res + xPixel] = 255; 
            }
        }
    }

    return matrix;
    






 }
  //

  void draw() override{
    Widget::draw();

    if (rastro.size() > 1){
        for(size_t i = 1; i < rastro.size(); i++){
            if(rastro[i].iniciaNovoTraco){
                continue;
            }

            C2D_DrawLine(
                rastro[i-1].x, rastro[i-1].y, corLinha,
                rastro[i].x, rastro[i].y, corLinha,
                espessuraLinha, z + 0.3f
            );
        }
    }
  }


  //const std::vector<Ponto2D>& getPontos() const {
    //    return rastro;
   // }

  void limpar() {
        rastro.clear();
        inativity = 0;
        writing = false;
    }


};
