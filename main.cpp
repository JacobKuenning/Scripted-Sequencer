#include "master.h"
#include "script.h"

int main(int argc, char* argv[]){
    script* s = new script(argv[1]);
    master m(s);

    delete s;

    return 0;
}