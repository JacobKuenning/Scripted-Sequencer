#include "message.h"
#include "strmidiconv.h"

message::message(int ch, int n, int v){
    channel = ch;
    note = n;
    velocity = v;
}

message::message(std::vector<std::string> args){
    int argc = args.size();
    channel = channelToInt(args[0]);
    
    if (args[1] == "OFF"){
        status = 0x80;
        note = 0;
        velocity = 0;
        return;
    }

    status = 0x90;
    note = noteToInt(args[1]);

    if (argc == 2){
        velocity = 127;
        return;
    } 

    velocity = std::stoi(args[2]);
}