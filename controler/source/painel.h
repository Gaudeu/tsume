/*
 * Hierarchical container structured according to the Composite pattern (derived from Widget)
 * - Groups multiple child graphical elements with batch rendering
 * - Manages the lifecycle and memory deallocation of its dependent elements
 * - Provides helper utility methods for geometric drawing
 */
#pragma once
#include "Widget.h"
#include <vector>

class Painel : public Widget {
protected:
    std::vector<Widget*> filhos; 

public:
    
    Painel(float x, float y, float z, float w, float h, u32 cor = C2D_Color32(0, 0, 0, 255))
        : Widget(x, y, z, w, h, cor, cor) {
    }

    
    ~Painel() {
        for (auto filho : filhos) {
            delete filho;
        }
        filhos.clear();
        
    }

    
    void adicionar(Widget* w) {
        filhos.push_back(w);
    }

    void draw() override {
        
        Widget::draw();

       
        for (auto filho : filhos) {
            filho->draw();
        }
    }

    
    void desenharLinha(float x1, float y1, float x2, float y2, float espessura, u32 cor) {
        C2D_DrawLine(x + x1, y + y1, cor, x + x2, y + y2, cor, espessura, 0.6f);
    }
};