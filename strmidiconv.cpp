#include <string>
#include <vector>

int channelToInt(std::string ch){
    ch.erase(0,2);
    return std::stoi(ch) - 1 ; // because channels start at 0
}

int noteToInt(std::string n){
    int octaveBegin = n.find_first_of("0123456789");
    std::string noteText = n.substr(0, octaveBegin);
    std::string octaveText = n.substr(octaveBegin, n.size()-octaveBegin);

    int octave = std::stoi(octaveText) + 1; // +1 is because midi officially has a "-1" octave, which is dumb
    char baseNote = noteText[0];
    int note = 0;

    switch (baseNote)
    {
    case 'c':
        note = 0;
        break;
    case 'd':
        note = 2;
        break;
    case 'e':
        note = 4;
        break;
    case 'f':
        note = 5;
        break;
    case 'g':
        note = 7;
        break;
    case 'a':
        note = 9;
        break;
    case 'b':
        note = 11;
        break;
    }

    if (noteText.size() > 1){
        char mod = noteText[1];
        switch (mod)
        {
        case 'b':
            note -= 1;
            break;
        case '#':
            note += 1;
            break;
        }
    }

    return note + (octave * 12); 
}