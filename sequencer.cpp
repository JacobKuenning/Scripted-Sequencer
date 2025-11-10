#include "sequencer.h"
#include "script.h"
#include "rtmidi/RtMidi.h"

#include <chrono>
#include <thread>
#include <iostream>

sequencer::sequencer(script* s){
    RtMidiOut *midiout = new RtMidiOut();
    midiout->openPort(0);

    while (pCounter < s->sLength){
        std::cout << s->getLine(pCounter) << std::endl;
        wait(1);
        pCounter++;
    }

    return;
}

void sequencer::wait(int s){
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return;
}