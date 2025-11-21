#pragma once
#include <string>
#include <vector>

class message{
    public:
        message(int ch, int n, int v);
        message(std::vector<std::string> args);
        int channel = 0;
        int note = 0;
        int velocity = 0;
        int status = 0;
};