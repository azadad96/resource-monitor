#include "Cpu.hpp"

Cpu::Cpu(SDL_Renderer *renderer, int x, int y, int w, int h) {
    std::ifstream cpufile("/proc/cpuinfo");
    std::string line;
    int threads = 0;
    while (std::getline(cpufile, line)) {
        if (line.substr(0, 9) == "processor")
            threads++;
    }
    cpufile.close();
    this->threads = threads;
    this->gridDims();
    this->queuesize = w / this->gridx / 3;
    for (int i = 0; i < this->getThreads(); i++) {
        std::deque<float> a;
        for (int j = 0; j < this->queuesize; j++)
            a.push_back(0.0f);
        this->usages.push_back(a);
    }
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    this->w_ = w / this->gridx;
    this->h_ = h / this->gridy;
    this->renderer = renderer;

    this->font = TTF_OpenFont("res/NotoSans-Regular.ttf", 12);
    if (!this->font) {
        fprintf(stderr, "Couldn't load font\n");
        exit(1);
    }
    for (int i = 0; i < this->getThreads(); i++) {
        std::string txt = "Thread #" + std::to_string(i);
        this->thread_labels.push_back(new Label(renderer, txt, this->font));
        this->freqs.push_back(0.0f);
    }
}

Cpu::~Cpu() {
    for (int i = 0; i < this->getThreads(); i++)
        delete this->thread_labels[i];
    TTF_CloseFont(font);
}

void Cpu::setSize(int x, int y, int w, int h) {
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    this->w_ = w / this->gridx;
    this->h_ = h / this->gridy;
    int new_queuesize = this->w_ / 3;
    while (new_queuesize > this->queuesize) {
        for (int i = 0; i < this->getThreads(); i++)
            this->usages[i].push_front(0.0f);
        this->queuesize++;
    }
    while (queuesize > new_queuesize) {
        for (int i = 0; i < this->getThreads(); i++)
            this->usages[i].pop_front();
        this->queuesize--;
    }
}

void Cpu::gridDims() {
    std::vector<int> factors;
    for (int i = 1; i <= this->getThreads(); i++) {
        if (this->getThreads() % i == 0)
            factors.push_back(i);
    }
    int a = factors[factors.size() / 2];
    int b = this->getThreads() / a;
    if (a > b) {
        this->gridx = a;
        this->gridy = b;
    } else {
        this->gridx = b;
        this->gridy = a;
    }
}

int Cpu::getThreads() {
    return this->threads;
}

void Cpu::renderUsage() {
    for (int i = 0; i < this->getThreads(); i++) {
        std::string txt = "Thread #" + std::to_string(i) + ": "
            + std::to_string(this->freqs[i]).substr(0, 4) + "GHz";
        if (this->thread_labels[i]->getText() != txt)
            this->thread_labels[i]->setText(this->renderer, txt);
    }

    for (int i = 0; i < this->getThreads(); i++) {
        if (this->usages[i].back() < 33.0f)
            SDL_SetRenderDrawColor(this->renderer, 0, 128, 0, 255);
        else if (this->usages[i].back() < 66.0f)
            SDL_SetRenderDrawColor(this->renderer, 192, 192, 0, 255);
        else
            SDL_SetRenderDrawColor(this->renderer, 128, 0, 0, 255);
        int x_ = i % this->gridx;
        int y_ = i / this->gridx;
        for (int j = 0; j < this->queuesize; j++) {
            SDL_Rect rect = {
                this->x + j * 3 + x_ * this->w_,
                this->y + (int) round(this->h_ - this->h_
                    * this->usages[i][j] / 100.0f) + y_ * this->h_,
                3,
                (int) round(this->h_ * this->usages[i][j] / 100.0f)
            };
            SDL_RenderFillRect(this->renderer, &rect);
        }
    }

    SDL_SetRenderDrawColor(this->renderer, 64, 64, 64, 255);
    for (int i = 1; i < this->gridx; i++) {
        int x_ = this->w_ * i;
        SDL_RenderDrawLine(this->renderer,
            this->x + x_, this->y,
            this->x + x_, this->y + this->h
        );
    }
    for (int i = 1; i < this->gridy; i++) {
        int y_ = this->h_ * i;
        SDL_RenderDrawLine(
            this->renderer,
            this->x, this->y + y_,
            this->x + this->w, this->y + y_
        );
    }

    for (int i = 0; i < this->getThreads(); i++) {
        int x_ = this->w_ * (i % this->gridx);
        int y_ = this->h_ * (i / this->gridx);
        this->thread_labels[i]->render(
            this->renderer,
            this->x + x_ + 5, this->y + y_ + 5,
            this->w_ - 10, this->h_ - 10
        );
    }
}

void Cpu::getUsage() {
    std::string data = RunCommand::run(
        "top -n 1 -1 | "
        "grep %Cpu | "
        "awk -F, '{print $4}' | "
        "sed 's/[ a-zA-Z]*$//g' | "
        "grep --color=never -o '[0-9]*\\.[0-9]*'"
    );
    std::string a = "";
    std::vector<float> res;
    for (int i = 0; i < data.length(); i++) {
        if (data.at(i) == '\n' || data.at(i) == '\0') {
            res.push_back(100.0f - std::stof(a));
            a.clear();
        } else
            a += data.at(i);
    }
    for (int i = 0; i < this->getThreads(); i++) {
        this->usages[i].push_back(res[i]);
        this->usages[i].pop_front();
    }

    data = RunCommand::run("cat /proc/cpuinfo | grep MHz | sed 's/[^0-9.]//g'");
    res.clear();
    for (int i = 0; i < data.length(); i++) {
        if (data.at(i) == '\n' || data.at(i) == '\0') {
            res.push_back(round(std::stof(a) / 10.0f) / 100.0f);
            a.clear();
        } else
            a += data.at(i);
    }
    for (int i = 0; i < this->getThreads(); i++) {
        this->freqs[i] = res[i];
    }
}