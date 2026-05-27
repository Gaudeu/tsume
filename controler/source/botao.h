#pragma once

#include "Widget.h"

class Botao : public Widget  {
public:
	C2D_Text* texto;

	Botao(float x, float y, float z, float w, float h,  C2D_Text* txt, u32 base, u32 foco, float r = 0.0f, bool circular = false)
		: Widget(x, y, z, w, h, base, foco, r, circular), texto(txt) {}

	void draw() override {
		Widget::draw();

		float tw, th;
		C2D_TextGetDimensions(texto, 0.6f, 0.6f, &tw, &th);
		if (circular)
		{
        C2D_DrawText(texto, C2D_WithColor, x - (tw) / 2, y - (th) / 2, 0.6f, 0.6f, 0.6f, C2D_Color32(0, 0, 0, 255));
		} 
		else 
		{
        C2D_DrawText(texto, C2D_WithColor, x + (w - tw) / 2, y + (h - th) / 2, 0.6f, 0.6f, 0.6f, C2D_Color32(0, 0, 0, 255));
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