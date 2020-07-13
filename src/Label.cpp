#include "Label.hpp"

Label::Label(
    SDL_Renderer *renderer,
    std::string text,
    TTF_Font *font,
    SDL_Color fg
) {
    this->text = text;
    this->color = fg;
    this->font = font;
    this->createTexture(renderer);
}

Label::~Label() {
    SDL_DestroyTexture(this->texture);
}

void Label::createTexture(SDL_Renderer *renderer) {
    SDL_Surface *surf = TTF_RenderText_Blended(
        this->font,
        this->text.c_str(),
        this->color
    );
    this->texture = SDL_CreateTextureFromSurface(renderer, surf);
    this->width = surf->w;
    this->height = surf->h;
    SDL_FreeSurface(surf);
}

int Label::getWidth() {
    return this->width;
}

int Label::getHeight() {
    return this->height;
}

std::string Label::getText() {
    return this->text;
}

SDL_Color Label::getColor() {
    return this->color;
}

void Label::setText(SDL_Renderer *renderer, std::string text) {
    this->text = text;
    this->createTexture(renderer);
}

void Label::setColor(SDL_Renderer *renderer, int r, int g, int b, int a) {
    this->color = (SDL_Color) {(Uint8) r, (Uint8) g, (Uint8) b, (Uint8) a};
    this->createTexture(renderer);
}

void Label::setColor(SDL_Renderer *renderer, SDL_Color color) {
    this->color = color;
    this->createTexture(renderer);
}

void Label::render(SDL_Renderer *renderer, int x, int y, int maxw, int maxh) {
    SDL_Rect dst = {x, y, this->width, this->height};
    if (maxw != -1 && maxw < this->width)
        dst.w = maxw;
    if (maxh != -1 && maxh < this->height)
        dst.h = maxh;
    SDL_Rect src = {0, 0, dst.w, dst.h};
    SDL_RenderCopy(renderer, this->texture, &src, &dst);
}