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

int updateInterval = 1000;
const int FPS = 60;
bool data_thread_running = true;
Cpu *cpu;
enum View {
    CPU_VIEW = 0,
    MEMORY_VIEW
};
View current = CPU_VIEW;

void dataGathering();

int main() {
    int width = 1000;
    int height = 500;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        fprintf(stderr, "An error occured initializing sdl.\n");
        return 1;
    }
    if (TTF_Init() != 0) {
        fprintf(stderr, "An error occured initializing sdl-ttf\n");
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "Resource monitor",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width, height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    SDL_Renderer *renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED
    );
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    int xoff = 40, yoff = 0;
    cpu = new Cpu(renderer, xoff, yoff, width - xoff, height - yoff);

    std::thread data_thread(dataGathering);

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
                        cpu->setSize(xoff, yoff, width - xoff, height - yoff);
                    }
                    break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 64, 64, 64, 255);
        SDL_RenderDrawLine(renderer, xoff, 0, xoff, height);

        if (current == CPU_VIEW)
            cpu->renderUsages();

        SDL_RenderPresent(renderer);

        frame_time = SDL_GetTicks() - frame_start;
        if (1000 / FPS > frame_time)
            SDL_Delay(1000 / FPS - frame_time);
        printf("%i\n", 1000 / FPS - frame_time);
    }

    data_thread_running = false;
    data_thread.join();
    delete cpu;
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    return 0;
}

void dataGathering() {
    unsigned int last_update = updateInterval + 1;
    while (data_thread_running) {
        if (SDL_GetTicks() - last_update > updateInterval) {
            last_update = SDL_GetTicks();
            if (current == CPU_VIEW)
                cpu->getCoreUsages();
        }
        SDL_Delay(1000 / FPS);
    }
}