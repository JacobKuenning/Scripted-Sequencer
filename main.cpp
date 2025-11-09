#include "rtmidi/RtMidi.h"
#include <iostream>
#include <thread>
#include <chrono>

int main(){
    RtMidiOut *midiout = new RtMidiOut();
    std::cout << midiout->getPortCount() << std::endl;
    midiout->openPort(0);

    std::vector<unsigned char> message;
    unsigned char channel = 0; // MIDI channel 1 (0 = channel 1)
    unsigned char note = 60;   // Middle C
    unsigned char velocity = 127; // Maximum velocity

    message.push_back(0x90 | channel); // 0x90 = Note On
    message.push_back(note);
    message.push_back(velocity);
    midiout->sendMessage(&message);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    message[0] = 0x80 | channel; // Note Off status
    message[2] = 0;              // velocity (optional)
    midiout->sendMessage(&message);

    return 0;
}