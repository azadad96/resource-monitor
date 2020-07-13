#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

class Label {
public:
    SDL_Texture *texture;
    int width;
    int height;

    Label(
    	SDL_Renderer *renderer,
    	std::string text,
    	TTF_Font *font,
    	SDL_Color fg = {255, 255, 255, 255}
    );
    ~Label();
    void render(
    	SDL_Renderer *renderer,
    	int x, int y,
    	int maxw = -1, int maxh = -1
    );
};