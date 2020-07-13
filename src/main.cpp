#include "Label.hpp"
#include "Cpu.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <fstream>
#include <utility>
#include <vector>
#include <deque>
#include <cstdio>
#include <cmath>
#include <thread>

int width = 1200;
int height = 600;
int updateInterval = 500;
const int FPS = 60;
SDL_Window *window;
SDL_Renderer *renderer;
bool data_thread_running = true;
std::vector<std::deque<float>> usages;

void dataGathering(int cores);
int getCpuCores();
std::vector<float> getCoreUsages();
std::string cmd(std::string cmd);
std::pair<int, int> gridDims(int elements);
void drawGrid(int x, int y);
void drawUsage(std::deque<float> usage, int qs, int x, int y, int w, int h);

int main() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        fprintf(stderr, "An error occured initializing sdl.\n");
        return 1;
    }
    if (TTF_Init() != 0) {
        fprintf(stderr, "An error occured initializing sdl-ttf\n");
        return 1;
    }

    window = SDL_CreateWindow(
        "Resource monitor",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width, height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    Cpu cpu;

    TTF_Font *font = TTF_OpenFont("NotoSans-Regular.ttf", 12);
    if (!font) {
        fprintf(stderr, "Couldn't load font\n");
        return 1;
    }
    std::vector<Label*> core_labels;
    for (int i = 0; i < cpu.getThreads(); i++) {
        std::string txt = "Thread #" + std::to_string(i);
        core_labels.push_back(new Label(renderer, txt, font));
    }

    std::pair<int, int> grid_dimensions = gridDims(cpu.getThreads());
    int gridx = grid_dimensions.first;
    int gridy = grid_dimensions.second;

    int queuesize = width / gridx / 3;
    for (int i = 0; i < cpu.getThreads(); i++) {
        std::deque<float> a;
        for (int j = 0; j < queuesize; j++)
            a.push_back(0.0f);
        usages.push_back(a);
    }

    std::thread data_thread(dataGathering, cpu.getThreads());

    unsigned int frame_start, frame_time;
    bool running = true;
    SDL_Event event;
    while (running) {
        frame_start = SDL_GetTicks();

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;

                case SDL_WINDOWEVENT:
                    if (
                        event.window.windowID == SDL_GetWindowID(window) &&
                        event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED
                    ) {
                        width = event.window.data1;
                        height = event.window.data2;
                        int new_queuesize = width / gridx / 3;
                        while (new_queuesize > queuesize) {
                            for (int i = 0; i < cpu.getThreads(); i++)
                                usages[i].push_front(0.0f);
                            queuesize++;
                        }
                        while (queuesize > new_queuesize) {
                            for (int i = 0; i < cpu.getThreads(); i++)
                                usages[i].pop_front();
                            queuesize--;
                        }
                    }
                    break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        for (int i = 0; i < cpu.getThreads(); i++) {
            drawUsage(
                usages[i], queuesize,
                i % gridx, i / gridx,
                width / gridx, height / gridy
            );
            int w = width / gridx, h = height / gridy;
            core_labels[i]->render(
                renderer,
                (i % gridx) * w + 5, (i / gridx) * h + 5,
                w - 10, h - 10
            );
        }
        drawGrid(gridx, gridy);

        SDL_RenderPresent(renderer);

        frame_time = SDL_GetTicks() - frame_start;
        if (1000 / FPS > frame_time)
            SDL_Delay(1000 / FPS - frame_time);
    }

    data_thread_running = false;
    data_thread.join();
    for (int i = 0; i < cpu.getThreads(); i++)
        delete core_labels[i];
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    return 0;
}

void dataGathering(int cores) {
    unsigned int last_update = updateInterval + 1;
    while (data_thread_running) {
        if (SDL_GetTicks() - last_update > updateInterval) {
            last_update = SDL_GetTicks();
            std::vector<float> usg = Cpu::getCoreUsages();
            for (int i = 0; i < cores; i++) {
                usages[i].push_back(usg[i]);
                usages[i].pop_front();
            }
        }
        SDL_Delay(1000 / FPS);
    }
}

std::pair<int, int> gridDims(int elements) {
    std::vector<int> factors;
    for (int i = 1; i <= elements; i++) {
        if (elements % i == 0)
            factors.push_back(i);
    }
    int a = factors[factors.size() / 2];
    int b = elements / a;
    std::pair<int, int> dims;
    if (a > b)
        dims = std::make_pair(a, b);
    else
        dims = std::make_pair(b, a);
    return dims;
}

void drawGrid(int x, int y) {
    SDL_SetRenderDrawColor(renderer, 64, 64, 64, 255);
    for (int i = 1; i < x; i++) {
        int w = width / x * i;
        SDL_RenderDrawLine(renderer, w, 0, w, height);
    }
    for (int i = 1; i < y; i++) {
        int h = height / y * i;
        SDL_RenderDrawLine(renderer, 0, h, width, h);
    }
}

void drawUsage(std::deque<float> usage, int qs, int x, int y, int w, int h) {
    if (usage.back() < 33.0f)
        SDL_SetRenderDrawColor(renderer, 0, 128, 0, 255);
    else if (usage.back() < 66.0f)
        SDL_SetRenderDrawColor(renderer, 192, 192, 0, 255);
    else
        SDL_SetRenderDrawColor(renderer, 128, 0, 0, 255);
    for (int i = 0; i < qs; i++) {
        SDL_Rect rect = {
            i * 3 + x * w,
            (int) round(h - h * usage[i] / 100.0f) + y * h,
            3,
            (int) round(h * usage[i] / 100.0f)
        };
        SDL_RenderFillRect(renderer, &rect);
    }
}