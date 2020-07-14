#include "Cpu.hpp"
#include "Memory.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
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
enum View {
    CPU_VIEW = 0,
    MEMORY_VIEW,
    VIEWS
};
View current = CPU_VIEW;
Cpu *cpu;
Memory *memory;

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
    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        fprintf(stderr, "An error occured initializing sdl-image\n");
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
    cpu = new Cpu(
        renderer, xoff, yoff, width - xoff, height - yoff
    );
    memory = new Memory(
        renderer, xoff, yoff, width - xoff, height - yoff
    );

    std::vector<SDL_Texture*> viewbtns;
    std::string img_paths[] = {"cpu", "memory"};
    for (int i = 0; i < sizeof(img_paths) / sizeof(img_paths[0]); i++) {
        std::string path = "res/" + img_paths[i] + ".png";
        viewbtns.push_back(IMG_LoadTexture(renderer, path.c_str()));
        if (!viewbtns[i]) {
            fprintf(stderr, "Couldn't load %s\n", path.c_str());
        }
    }

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
                        memory->setSize(
                            xoff, yoff, width - xoff, height - yoff
                        );
                    }
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    if (
                        event.button.button == SDL_BUTTON_LEFT &&
                        event.button.x < xoff
                    )
                        for (int i = 0; i < viewbtns.size(); i++)
                            if (
                                event.button.y > i * xoff &&
                                event.button.y < (i + 1) * xoff
                            )
                                current = (View) i;
                    break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 64, 64, 64, 255);
        SDL_RenderDrawLine(renderer, xoff, 0, xoff, height);

        if (current == CPU_VIEW)
            cpu->renderUsage();
        else if (current == MEMORY_VIEW)
            memory->renderUsage();

        for (int i = 0; i < viewbtns.size(); i++) {
            SDL_Rect dst = {0, xoff * i, xoff, xoff};
            SDL_RenderCopy(renderer, viewbtns[i], NULL, &dst);
        }

        SDL_RenderPresent(renderer);

        frame_time = SDL_GetTicks() - frame_start;
        if (1000 / FPS > frame_time)
            SDL_Delay(1000 / FPS - frame_time);
    }

    data_thread_running = false;
    data_thread.join();
    for (int i = 0; i < viewbtns.size(); i++)
        SDL_DestroyTexture(viewbtns[i]);
    delete cpu;
    delete memory;
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    IMG_Quit();
    SDL_Quit();
    return 0;
}

void dataGathering() {
    unsigned int last_update = updateInterval + 1;
    while (data_thread_running) {
        if (SDL_GetTicks() - last_update > updateInterval) {
            last_update = SDL_GetTicks();
            if (current == CPU_VIEW)
                cpu->getUsage();
            else if (current == MEMORY_VIEW)
                memory->getUsage();
        }
        SDL_Delay(1000 / FPS);
    }
}