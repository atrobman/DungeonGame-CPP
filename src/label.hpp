#ifndef LABEL_H
#define LABEL_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <string>

class Label {

public:

    Label();
    Label(TTF_Font* font);
    Label(TTF_Font* font, std::string text, SDL_Renderer* gRenderer);
    ~Label();
    void setText(SDL_Renderer* gRenderer, std::string text);
    void draw(SDL_Renderer* gRenderer, int x, int y);
    void setFont(SDL_Renderer* gRenderer, TTF_Font* font);
    void setBG(SDL_Color color);
    void setTextColor(SDL_Renderer* gRenderer, SDL_Color color);

private:

    void makeTexture(SDL_Renderer* gRenderer);

    std::string displayText;
    SDL_Texture* texture;
    TTF_Font* displayFont;
    int w;
    int h;
    SDL_Color bg;
    SDL_Color textColor;

};

#endif