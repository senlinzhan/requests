#include "Utils.hpp"

#include <assert.h>

using namespace requests;

int main(int argc, char *argv[])
{
    auto tokens = splitString("Hello\nWorld", "\n");
    assert(tokens.size() == 2);
    assert(tokens[0] == "Hello");
    assert(tokens[1] == "World");
    
    return 0;
}

