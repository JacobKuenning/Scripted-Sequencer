#pragma once
#include "errors.h"
#include "lineutils.h"
#include <string>
#include <vector>
#include <iostream>

bool validMessageArgs(std::vector<std::string> args);
bool validNote(std::string note);
bool validCC(std::string cc);
bool validChannel(std::string ch);
bool validVelocity(std::string v);