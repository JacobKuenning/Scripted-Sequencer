#include "cclerper.h"
#include <cmath>
#include <thread>
#include <chrono>
#include <rtmidi/RtMidi.h>
#include "globals.h"

cclerper::cclerper(int ch, int cc, int start, int end, float t, RtMidiOut* m){
    channel = ch;
    controlChange = cc;
    ccStart = start;
    ccEnd = end;
    overTime = t;
    midiout = m;
}

void cclerper::interpolate(){
    int ccVal = 0;
    int lastVal = 0;

    float l = 0; // interpolate value, is between 0 and 1

    float inc = 1.0f / (overTime * 1000.0f); // how much to increment by each time. will take 1 seconds for l to equal 1

    while (l < 1.0f && running){
        ccVal = std::round(lerp(ccStart,ccEnd,l));
        if (ccVal != lastVal){ // only send a message when we've had a whole integer change in ccVal
            sendCCMessage(ccVal);
        }
        lastVal = ccVal;
        l += inc;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

float cclerper::lerp(float a, float b, float t){
    return a + (b - a) * t;
}

void cclerper::sendCCMessage(int value){
    std::vector<unsigned char> msg(3);
    msg[0] = 0xB0 + (channel);
    msg[1] = controlChange;
    msg[2] = value;

    midiMx.lock();
    midiout->sendMessage(&msg);
    midiMx.unlock();
}