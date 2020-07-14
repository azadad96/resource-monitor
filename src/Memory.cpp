#include "Memory.hpp"

Memory::Memory(SDL_Renderer *renderer, int x, int y, int w, int h) {
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    this->renderer = renderer;

    std::string data = RunCommand::run(
        "free -m | grep Mem | awk '{print $2}'"
    );
    std::string a = "";
    std::vector<int> res;
    for (int i = 0; i < data.length(); i++) {
        if (data.at(i) == '\n' || data.at(i) == '\0') {
            res.push_back(std::stof(a));
            a.clear();
        } else
            a += data.at(i);
    }
    this->totalMemory = round(res[0] / 100) / 10;
    this->totalSwap = round(res[1] / 100) / 10;

    this->queuesize = w / 3;
    for (int i = 0; i < this->queuesize; i++) {
        this->usedMemory.push_back(0.0f);
        this->usedSwap.push_back(0.0f);
    }

    this->font = TTF_OpenFont("res/NotoSans-Regular.ttf", 12);
    if (!this->font) {
        fprintf(stderr, "Couldn't load font\n");
        exit(1);
    }

    this->memoryLabel = new Label(renderer, "a", this->font);
    this->swapLabel = new Label(renderer, "b", this->font);
}

Memory::~Memory() {
    delete this->memoryLabel;
    delete this->swapLabel;
    TTF_CloseFont(this->font);
}

void Memory::setSize(int x, int y, int w, int h) {
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    int new_queuesize = this->w / 3;
    while (new_queuesize > this->queuesize) {
        this->usedMemory.push_front(0.0f);
        this->usedSwap.push_front(0.0f);
        this->queuesize++;
    }
    while (queuesize > new_queuesize) {
        this->usedMemory.pop_front();
        this->usedSwap.pop_front();
        this->queuesize--;
    }
}

void Memory::renderUsage() {
    std::string used = std::to_string(this->usedMemory.back());
    used = used.substr(0, used.find(".") + 2);
    std::string total = std::to_string(this->totalMemory);
    total = total.substr(0, total.find(".") + 2);
    std::string txt = "Memory: "  + used + "GB/" + total + "GB"; 
    if (memoryLabel->getText() != txt)
        memoryLabel->setText(this->renderer, txt);

    used = std::to_string(this->usedSwap.back());
    used = used.substr(0, used.find(".") + 2);
    total = std::to_string(this->totalSwap);
    total = total.substr(0, total.find(".") + 2);
    txt = "Swap: "  + used + "GB/" + total + "GB"; 
    if (swapLabel->getText() != txt)
        swapLabel->setText(this->renderer, txt);

    if (this->usedMemory.back() / this->totalMemory < 0.33f)
        SDL_SetRenderDrawColor(this->renderer, 0, 128, 0, 255);
    else if (this->usedMemory.back() / this->totalMemory < 0.66f)
        SDL_SetRenderDrawColor(this->renderer, 192, 192, 0, 255);
    else
        SDL_SetRenderDrawColor(this->renderer, 128, 0, 0, 255);
    for (int i = 0; i < this->queuesize; i++) {
        SDL_Rect rect = {
            this->x + i * 3,
            this->y + (int) round(this->h / 2 - this->h / 2
                * this->usedMemory[i] / this->totalMemory),
            3,
            (int) round(this->h / 2 * this->usedMemory[i] / this->totalMemory)
        };
        SDL_RenderFillRect(this->renderer, &rect);
    }
    
    if (this->usedSwap.back() / this->totalSwap < 0.33f)
        SDL_SetRenderDrawColor(this->renderer, 0, 128, 0, 255);
    else if (this->usedSwap.back() / this->totalSwap < 0.66f)
        SDL_SetRenderDrawColor(this->renderer, 192, 192, 0, 255);
    else
        SDL_SetRenderDrawColor(this->renderer, 128, 0, 0, 255);
    for (int i = 0; i < this->queuesize; i++) {
        SDL_Rect rect = {
            this->x + i * 3,
            this->y + (int) round(this->h / 2 - this->h / 2
                * this->usedSwap[i] / this->totalSwap),
            3,
            (int) round(this->h / 2 * this->usedSwap[i] / this->totalSwap)
        };
        SDL_RenderFillRect(this->renderer, &rect);
    }

    memoryLabel->render(
        this->renderer, this->x + 5, this->y + 5, this->w - 10, this->h / 2 - 10
    );
    swapLabel->render(
        this->renderer,
        this->x + 5, this->y + this->h / 2 + 5,
        this->w - 10, this->h / 2 - 10
    );
    SDL_SetRenderDrawColor(this->renderer, 64, 64, 64, 255);
    SDL_RenderDrawLine(this->renderer,
        this->x, this->y + this->h / 2,
        this->x + this->w, this->y + this->h / 2
    );
}

void Memory::getUsage() {
    std::string data = RunCommand::run(
        "free -m | grep Mem | awk '{print $3}'"
    );
    std::string a = "";
    std::vector<float> res;
    for (int i = 0; i < data.length(); i++) {
        if (data.at(i) == '\n' || data.at(i) == '\0') {
            res.push_back(std::stof(a));
            a.clear();
        } else
            a += data.at(i);
    }
    this->usedMemory.push_back(round(res[0] / 100) / 10);
    this->usedMemory.pop_front();
    this->usedSwap.push_back(round(res[1] / 100) / 10);
    this->usedSwap.pop_front();
}