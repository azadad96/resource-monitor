#include "Cpu.hpp"

Cpu::Cpu() {
	std::ifstream cpufile("/proc/cpuinfo");
    std::string line;
    int threads = 0;
    while (std::getline(cpufile, line)) {
        if (line.substr(0, 9) == "processor")
            threads++;
    }
    cpufile.close();
    this->threads = threads;
}

int Cpu::getThreads() {
	return this->threads;
}

std::vector<float> Cpu::getCoreUsages() {
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
            res.push_back(100 - std::stod(a));
            a.clear();
        } else
            a += data.at(i);
    }
    return res;
}