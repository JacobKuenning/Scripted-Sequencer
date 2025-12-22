#include "master.h"
#include "script.h"
#include "errors.h"
#include <iostream>

int main(int argc, char* argv[]){
    if (argc < 2){
        error("No file name given.");
        return 0;
    }

    if (argc > 2){
        error("Too many arguments.");
        return 0;
    }

    std::cout << "Program is running. Press 'Q' to stop program." << std::endl;

    script* s = new script(argv[1]);
    master m(s);

    delete s;

    return 0;
}