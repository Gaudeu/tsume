/*
 * Fundamental base class for visual elements and graphical interface components
 * - Stores spatial properties (x, y, z, width, height, radius) and color/focus states
 * - Supports drawing 2D geometric primitives (rectangles and circles) integrated with Citro2D
 */
#pragma once
#include <citro2d.h>
#include <string>
#include "Common.h"

class Widget {
public:
	float x, y, w, h, r, z;
	u32 corFundo, corFoco;
	bool selecionado = false;
	bool circular = false;

	Widget(float x, float y, float z, float w, float h, u32 corFundo, u32 corFoco, float r = 0.0f, bool circular = false)
		: x(x), y(y), z(z), w(w), h(h), corFundo(corFundo), corFoco(corFoco),r(r), circular(circular) {
	}
	virtual ~Widget() {}


	virtual void draw(){
		u32 cor = selecionado ? corFoco : corFundo;
		if(circular)
		{
        
		 C2D_DrawCircle(x, y, z, r, cor, cor, cor, cor); 
		}
		else
		{
         
		 C2D_DrawRectangle(x, y, z, w, h, cor, cor, cor, cor);
		}
		
	}

	
};