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
            inativity = 0;
            return true;
        }

    }
    return false;
  }

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