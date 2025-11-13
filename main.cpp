#include "script.h"
#include "sequencer.h"

int main(int argc, char** argv){
    script s(argv[1]);
    sequencer seq(&s);
    int n = 0;
    return 0;
}