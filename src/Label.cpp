#include "Label.hpp"

Label::Label(SDL_Texture *texture, int width, int height) {
	this->texture = texture;
	this->width = width;
	this->height = height;
}

Label::~Label() {
	SDL_DestroyTexture(this->texture);
}