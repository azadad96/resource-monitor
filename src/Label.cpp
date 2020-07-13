#include "Label.hpp"

Label::Label(
	SDL_Renderer *renderer,
	std::string text,
	TTF_Font *font,
	SDL_Color fg
) {
    SDL_Surface *surf = TTF_RenderText_Blended(font, text.c_str(), fg);
	this->texture = SDL_CreateTextureFromSurface(renderer, surf);
	this->width = surf->w;
	this->height = surf->h;
	SDL_FreeSurface(surf);
}

Label::~Label() {
	SDL_DestroyTexture(this->texture);
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