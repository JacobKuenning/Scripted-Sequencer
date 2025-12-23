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

bool varError(void* ptr, std::string vname){
    if (ptr == nullptr){
        warning("Variable \"%s\" does not exist.", vname);
        variableMx.unlock();
        return true;
    }
    return false;
}

bool argcError(std::string fname, int argc, int val){
    if (argc == 0 || argc > val){
        warning("%s takes %d arguments.", fname.c_str(), val);
        return true;
    }
    return false;
}

bool argcError(std::string fname, int argc, int v1, int v2){
    if (argc < v1 || argc > v2){
        warning("%s takes %d or %d arguments.", fname.c_str(), v1, v2);
        return true;
    }
    return false;
}
