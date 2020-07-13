#include "RunCommand.hpp"

std::string RunCommand::run(std::string cmd) {
	FILE *f = popen(cmd.c_str(), "r");
    if (!f) {
        fprintf(stderr, "Couldn't execute command\n");
        exit(1);
    }
    std::string res = "";
    char buff[128];
    while (fgets(buff, sizeof(buff), f) != NULL) {
        res += buff;
    }
    return res;
}