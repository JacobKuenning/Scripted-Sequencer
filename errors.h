#pragma once
#include <iostream>
#include <string>
#include <vector>

// errors worth terminating the program
void error(const char* format, ...);

// errors not worth terminating the program
void warning(const char* format, ...);
