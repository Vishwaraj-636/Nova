#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "lexer.h"
#include "interpreter.h"

int main(int argc, char* argv[])
{
    const std::string sourcePath = argc > 1 ? argv[1] : "examples/demo.nova";
    std::ifstream file(sourcePath);

    if(!file)
    {
        std::cout << "Cannot open source file: " << sourcePath << "\n";
        return 1;
    }

    std::stringstream buffer;

    buffer << file.rdbuf();

    Lexer lexer(buffer.str());

    auto tokens = lexer.scanTokens();

    Interpreter interpreter;

    if(!interpreter.execute(tokens))
    {
        return 1;
    }

    return 0;
}