#include "script.h"
#include "sequencer.h"
#include "rtmidi/RtMidi.h"
#include "globals.h"
#include <thread>
#include <vector>
#include <chrono>
#include <functional>
#include <iostream>

std::vector<std::thread> threads;
std::vector<sequencer*> seqs;
script* s;
RtMidiOut* midiout = new RtMidiOut;
bool done = false;
std::mutex outputMx;
std::mutex vectorMx;
std::mutex midiMx;

// create new sequencer, start it at line i, it will run until it reaches an @ END
void branch(int i){
    sequencer* seq = new sequencer(s,false,i,branch, midiout);
    vectorMx.lock();
    seqs.push_back(seq);
    std::thread seqThread;
    seqThread = std::thread(&sequencer::run, seq);
    threads.push_back(std::move(seqThread));
    vectorMx.unlock();
    return;
}

int main(int argc, char** argv){
    midiout->openPort(0);
    s = new script(argv[1]); // create script
    sequencer* seq = new sequencer(s,true,0,branch, midiout);
    seqs.push_back(seq);
    std::thread seqThread;
    seqThread = std::thread(&sequencer::run, seq);
    threads.push_back(std::move(seqThread));

    // main thread periodically loops through, joins finished threads and frees memory
    while(!threads.empty()){
        vectorMx.lock();
        for (int i= 0; i < seqs.size(); i++){
            if (seqs[i]->running == false){
                if (threads[i].joinable()){
                    threads[i].join();
                    delete seqs[i];
                    seqs.erase(seqs.begin()+i);
                    threads.erase(threads.begin() + i);
                }
            }
        }
        vectorMx.unlock();
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }

    delete s;

    //delete midiout;

    return 0;
}