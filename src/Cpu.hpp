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
	TTF_Font *font;
	SDL_Renderer *renderer;
	void gridDims();

public:
	Cpu(SDL_Renderer *renderer, int x, int y, int w, int h);
	~Cpu();
	int getThreads();
	void renderUsages();
	void setSize(int x, int y, int w, int h);
	void getCoreUsages();
};

#endif