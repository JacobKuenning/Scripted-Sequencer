#include "script.h"
#include "sequencer.h"
#include "message.h"
#include "rtmidi/RtMidi.h"
#include "globals.h"
#include <thread>
#include <vector>
#include <chrono>
#include <functional>
#include <iostream>
#include <termios.h>
#include <unistd.h>

std::vector<std::thread> threads;
std::vector<sequencer*> seqs;
script* s;
RtMidiOut* midiout = new RtMidiOut;
bool done = false;
std::mutex outputMx;
std::mutex vectorMx;
std::mutex midiMx;

void input(){
    while(!done){
        int c = getchar();
        if (c=='q'){
            vectorMx.lock();
            for (sequencer* n : seqs){
                n->running = false;
            }
            done = true;
            vectorMx.unlock();
        }
    }
}

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

void setRawMode(bool enable) { 
    static termios oldt; 
    termios newt; 
    if (enable) { 
        tcgetattr(STDIN_FILENO, &oldt); 
        newt = oldt; 
        newt.c_lflag &= ~(ICANON | ECHO); // no buffering, no echo tcsetattr(STDIN_FILENO, TCSANOW, &newt); } else { tcsetattr(STDIN_FILENO, TCSANOW, &oldt); } }
        tcsetattr(STDIN_FILENO, TCSANOW, &newt); 
    } else { 
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt); 
    } 
}

void killAllMidi(){
    std::vector<message> offMessages;

    for (int i = 0; i < 16; i++){
        for (int n = 0; n < 108; n++)
        {
            std::vector<unsigned char> msg(3);
            msg[0] = i | 0x80;
            msg[1] = n;
            msg[2] = 0;
            midiout->sendMessage(&msg);
        }
    }
}

int main(int argc, char** argv){
    midiout->openPort(0);
    setRawMode(true);

    std::thread inputThread = std::thread(&input);

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

    inputThread.join();
    killAllMidi();

    delete s;

    //delete midiout;
    setRawMode(false);
    return 0;
}