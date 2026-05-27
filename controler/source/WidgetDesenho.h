#pragma once
#include <Widget.h>
#include <vector>
#include <cmath>


class WidgetDesenho: public Widget 
{
 private:
  std::vector<Ponto2D> rastro;
  u32 corLinha;
  float espessuraLinha;

 public:
  WidgetDesenho(float x, float y, float z, float w, float h, u32 corFundo, u32 corLinha)
    : Widget(x, y, z, w, h, corFundo, corFundo), corLinha(corLinha), espessuraLinha(3.0f) {}
  
  bool contemPonto(float tx, float ty){
     return(tx >= x && tx <= x + w && ty >= y && ty <= y + h);
  }

  bool update(const InputPacket& packet) {

    if (packet.keysHeld & KEY_TOUCH){
        if (contemPonto(packet.touchX, packet.touchY)){
            Ponto2D novoPonto = { (float)packet.touchX, (float)packet.touchY};

            //adiciona apenas se o dedo se moveo 3px
            if(rastro.empty()){
                rastro.push_back(novoPonto);
            } else {
                Ponto2D ultimo = rastro.back();
                float dist = std::sqrt(std::pow(novoPonto.x - ultimo.x, 2) + std::pow(novoPonto.y - ultimo.y, 2));
                if(dist > 3.0f){
                    rastro.push_back(novoPonto);
                }
            }
        }
    }

    if ((packet.keysUp & KEY_TOUCH) && !rastro.empty()){
        return true;
    }

    return false;

  }

  void draw() override{
    Widget::draw();

    if (rastro.size() > 1){
        for(size_t i = 1; i < rastro.size(); i++){
            C2D_DrawLine(
                rastro[i-1].x, rastro[i-1].y, corLinha,
                rastro[i].x, rastro[i].y, corLinha,
                espessuraLinha, z + 0.1f
            );
        }
    }
  }


  //const std::vector<Ponto2D>& getPontos() const {
    //    return rastro;
   // }

  void limpar() {
        rastro.clear();
    }


};