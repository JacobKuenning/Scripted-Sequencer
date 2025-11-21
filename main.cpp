#include "script.h"
#include "sequencer.h"
#include <thread>
#include <vector>
#include <chrono>
#include <functional>
#include <iostream>

std::vector<std::thread> threads;
std::vector<sequencer*> seqs;
std::string sname;
script* s;

// create new sequencer, start it at line i, it will run until it reaches an @ END
void branch(int i){
    sequencer* seq = new sequencer(s,false,i,branch);
    std::thread seqThread(&sequencer::run, seq);
    threads.push_back(std::move(seqThread));
    std::cout << "SUCCESSFUL BRANCH" << std::endl;

    return;
}

int main(int argc, char** argv){
    s = new script(argv[1]); // create script
    sequencer* seq = new sequencer(s,true,0,branch);
    std::thread seqThread(&sequencer::run, seq);
    threads.push_back(std::move(seqThread)); // cant copy threads

    for (std::thread &t : threads){
        t.join();
    }

    return 0;
}