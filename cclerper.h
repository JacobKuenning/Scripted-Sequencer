#pragma once
#include "rtmidi/RtMidi.h"

class cclerper{
    public:
    int channel = 0;
    int controlChange = 0;
    
    int ccStart = 0;
    int ccEnd = 0;
    float overTime = 0;

    bool running = true;
    RtMidiOut* midiout = nullptr;

    cclerper(int ch, int cc, int start, int end, float t, RtMidiOut* m);
    void interpolate();
    float lerp(float a, float b, float t);
    void sendCCMessage(int value);
};