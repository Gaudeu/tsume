#pragma once
#include <citro2d.h> 

class conteudoBotao{
    public:
     virtual ~conteudoBotao(){}
     virtual void draw(float btnX, float btnY, float btnW, float btnH, bool circular, bool selecionado) = 0;
     virtual void aoClicar() {}
};

class ConteudoTexto : public conteudoBotao {
  private:
    C2D_Text* texto;
    float escala;
    bool centralizar;
  public:
    ConteudoTexto(C2D_Text* txt, float escalaTexto = 0.6f, bool alinhamentoCentral = true)
     : texto(txt),escala(escalaTexto), centralizar(alinhamentoCentral) {}
    
    void draw(float bx, float by, float bw, float bh, bool circular, bool selecionado) override {
        float tw, th;
        C2D_TextGetDimensions(texto, escala, escala, &tw, &th);
        
        if (circular) {
            
            C2D_DrawText(texto, C2D_WithColor, bx - tw / 2, by - th / 2, 0.6f, escala, escala, C2D_Color32(0, 0, 0, 255));
        } else {
            
            float posX;
            
            
            if (centralizar) {
                posX = bx + (bw - tw) / 2; 
            } else {
                posX = bx + 5.0f; 
            }

            
            C2D_DrawText(texto, C2D_WithColor, posX, by + (bh - th) / 2, 0.6f, escala, escala, C2D_Color32(0, 0, 0, 255));
        }
    }
};

class ConteudoImagem : public conteudoBotao {
private:
    C2D_Image* imagemOriginal;
    C2D_Image* imagemAlternativa;
    bool ativo;

public:
    ConteudoImagem(C2D_Image* original, C2D_Image* alternativa) 
        : imagemOriginal(original), imagemAlternativa(alternativa), ativo(false) {}

    void aoClicar() override {
        ativo = !ativo;
    }

    void draw(float bx, float by, float bw, float bh, bool circular, bool selecionado) override {
        
        C2D_Image* imgParaDesenhar = ativo ? imagemAlternativa : imagemOriginal;
        
        if (circular) {
            C2D_DrawImageAt(*imgParaDesenhar, bx - (bw / 2), by - (bh / 2), 1.0f, NULL, 1.0f, 1.0f);
        } else {
            C2D_DrawImageAt(*imgParaDesenhar, bx, by, 1.0f, NULL, 1.0f, 1.0f);
        }
    }
};