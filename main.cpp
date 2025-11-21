#include "script.h"
#include "sequencer.h"

int main(int argc, char** argv){
    script s(argv[1]);
    sequencer seq(&s);
    seq.run();
    return 0;
}

