#include "script.h"
#include "sequencer.h"

int main(int argc, char** argv){
    script s(argv[1]);
    sequencer seq(&s);
    return 0;
}

