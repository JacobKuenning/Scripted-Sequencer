#pragma once
#include "globals.h"
#include "lineutils.h"
#include <iostream>
#include <string>
#include <vector>

// errors worth terminating the program
void error(const char* format, ...);

// errors not worth terminating the program
void warning(const char* format, ...);

// returns true if ptr is nullptr
bool varError(void* ptr, std::string vname);

// returns true if argc isn't the correct amount
bool argcError(std::string fname, int argc, int val);

// returns true if argc isn't either other argument
bool argcError(std::string fname, int argc, int v1, int v2);
