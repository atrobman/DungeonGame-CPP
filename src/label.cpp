#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string>

#include "label.hpp"

Label::Label() {
    displayFont = NULL;
    displayText = "";
    texture = NULL;
    bg = {0x00, 0x00, 0x00, 0x00};
    textColor = {0xFF, 0xFF, 0xFF, 0xFF};
    w = 0;
    h = 0;
}
Label::Label(TTF_Font* font) {

    displayFont = font;
    displayText = "";
    texture = NULL;
    bg = {0x00, 0x00, 0x00, 0x00};
    textColor = {0xFF, 0xFF, 0xFF, 0xFF};
    w = 0;
    h = 0;
}

Label::Label(TTF_Font* font, std::string text, SDL_Renderer* gRenderer) {

    displayFont = font;
    displayText = text;
    bg = {0x00, 0x00, 0x00, 0x00};
    textColor = {0xFF, 0xFF, 0xFF, 0xFF};

	SDL_Surface* s_text = TTF_RenderText_Blended(displayFont, displayText.c_str(), textColor);
	texture = SDL_CreateTextureFromSurface( gRenderer, s_text );
    w = s_text->w;
    h = s_text->h;
	SDL_FreeSurface(s_text);
}

Label::~Label() {

    if (texture != NULL) {
        SDL_DestroyTexture(texture);
    }

    displayText = "";
    texture = NULL;
    displayFont = NULL;
}

void Label::setFont(SDL_Renderer* gRenderer, TTF_Font* font) {
    
    if (font != displayFont) {
        displayFont = font;
        makeTexture(gRenderer);
    }

}

void Label::setText(SDL_Renderer* gRenderer, std::string text) {

    if (displayText.compare(text) != 0) {
        displayText = text;
        makeTexture(gRenderer);
    }

}

void Label::makeTexture(SDL_Renderer* gRenderer) {

    if (displayFont != NULL) {
        SDL_Surface* s_text = TTF_RenderText_Blended(displayFont, displayText.c_str(), textColor);
        texture = SDL_CreateTextureFromSurface( gRenderer, s_text );
        w = s_text->w;
        h = s_text->h;
        SDL_FreeSurface(s_text);
    }

}

void Label::draw(SDL_Renderer* gRenderer, int x, int y) {

    if (texture != NULL) {
        SDL_Rect r = {x, y, w, h};
        SDL_SetRenderDrawColor( gRenderer, bg.r, bg.g, bg.b, bg.a );
        SDL_RenderFillRect( gRenderer, &r );
        SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0x00, 0xFF );
        SDL_RenderCopy( gRenderer, texture, NULL, &r );
    }
}

void Label::setBG(SDL_Color color) {
    bg = color;
}

void Label::setTextColor(SDL_Renderer* gRenderer, SDL_Color color) {

    if ( color.r != textColor.r || color.g != textColor.g || color.b != textColor.b || color.a != textColor.a ) {
        textColor = color;
        makeTexture(gRenderer);
    }

}