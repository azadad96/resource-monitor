#ifndef CPU
#define CPU

#include "RunCommand.hpp"
#include <vector>
#include <fstream>
#include <string>

class Cpu {
private:
	int threads;

public:
	Cpu();
	int getThreads();
	static std::vector<float> getCoreUsages();
};

#endif