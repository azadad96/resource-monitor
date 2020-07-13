#include <SDL2/SDL.h>

class Label {
public:
    SDL_Texture *texture;
    int width;
    int height;

    Label(SDL_Texture *texture, int width, int height);
    ~Label();
};