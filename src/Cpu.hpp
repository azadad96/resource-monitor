#ifndef CPU
#define CPU

#include "RunCommand.hpp"
#include "Label.hpp"
#include <SDL2/SDL.h>
#include <vector>
#include <deque>
#include <fstream>
#include <string>
#include <cmath>
#include <cstdio>

class Cpu {
private:
    int threads;
    int gridx, gridy;
    int x, y, w, h;
    int w_, h_;
    int queuesize;
    std::vector<std::deque<float>> usages;
    std::vector<Label*> thread_labels;
    std::vector<float> freqs;
    TTF_Font *font;
    SDL_Renderer *renderer;
    void gridDims();
    int getThreads();

public:
    Cpu(SDL_Renderer *renderer, int x, int y, int w, int h);
    ~Cpu();
    void renderUsage();
    void setSize(int x, int y, int w, int h);
    void getUsage();
};

#endif