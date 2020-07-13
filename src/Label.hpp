#ifndef LABEL
#define LABEL

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

class Label {
public:
private:
    SDL_Texture *texture;
    std::string text;
    int width;
    int height;
    SDL_Color color;
    TTF_Font *font;
    void createTexture(SDL_Renderer *renderer);

public:
	Label(
    	SDL_Renderer *renderer,
    	std::string text,
    	TTF_Font *font,
    	SDL_Color fg = {255, 255, 255, 255}
    );
    ~Label();
	int getWidth();
	int getHeight();
	std::string getText();
	SDL_Color getColor();
	void setText(SDL_Renderer *renderer, std::string text);
	void setColor(SDL_Renderer *renderer, int r, int g, int b, int a = 255);
	void setColor(SDL_Renderer *renderer, SDL_Color color);
    void render(
    	SDL_Renderer *renderer,
    	int x, int y,
    	int maxw = -1, int maxh = -1
    );
};

#endif