#include "errors.h"
#include <cstdarg>

void error(const char* format, ...) {
    va_list args;
    va_start(args, format);
    fprintf(stderr, "\033[31mERROR: "); // make error text red
    vfprintf(stderr, format, args);
    fprintf(stderr, "\033[0m\n");
    va_end(args);
    exit(0);
}

void warning(const char* format, ...){
    va_list args;
    va_start(args, format);
    fprintf(stderr, "\033[31mWARNING: ");
    vfprintf(stderr, format, args);
    fprintf(stderr, "\033[0m\n");
    va_end(args);
}
