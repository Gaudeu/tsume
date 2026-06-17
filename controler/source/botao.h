#pragma once

#include "Widget.h"
#include "conteudoBotao.h"

class Botao : public Widget  {
public:
	conteudoBotao* conteudo;

	Botao(float x, float y, float z, float w, float h,  conteudoBotao* cont, u32 base, u32 foco, float r = 0.0f, bool circular = false)
		: Widget(x, y, z, w, h, base, foco, r, circular), conteudo(cont) {}


	void aoClicar() {
        //revisar depois

        if(conteudo != nullptr){
			conteudo->aoClicar();
		}
    }

	void draw() override {
		Widget::draw();

		if (conteudo != nullptr) {
            conteudo->draw(x, y, w, h, circular, selecionado); 
        }
		
	}

	bool foiTocado(u16 tx, u16 ty) {
		if (circular)
		{
        return ((tx - x)*(tx - x) +(ty - y)*(ty - y)) <= (r*r);
		} 
		else 
		{
        return (tx >= x && tx <= x + w && ty >= y && ty <= y + h);
		}
		
	}
	//entender melhor o this-> ou header->draw (operador ->)
	void setPosicao(float x, float y) {
		this->x = x;
		this->y = y;
	}
   
};