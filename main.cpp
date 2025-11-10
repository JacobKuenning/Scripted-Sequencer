#include "rtmidi/RtMidi.h"
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include "script.h"

int main(int argc, char** argv){
    RtMidiOut *midiout = new RtMidiOut();
    midiout->openPort(0);

    script s(argv[1]);

    return 0;
}