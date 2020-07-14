#ifndef MEMORY
#define MEMORY

#include "RunCommand.hpp"
#include "Label.hpp"
#include <SDL2/SDL.h>
#include <vector>
#include <deque>
#include <string>
#include <cmath>
#include <cstdio>

class Memory {
private:
    int x, y, w, h;
    float totalMemory, totalSwap;
    int queuesize;
    std::deque<float> usedMemory;
    std::deque<float> usedSwap;
    Label *memoryLabel, *swapLabel;
    TTF_Font *font;
    SDL_Renderer *renderer;


public:
    Memory(SDL_Renderer *renderer, int x, int y, int w, int h);
    ~Memory();
    void renderUsage();
    void getUsage();
    void setSize(int x, int y, int w, int h);
};

#endif