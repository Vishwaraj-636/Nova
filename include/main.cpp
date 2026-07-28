#include <fstream>
#include <iostream>
#include <sstream>

#include "lexer.h"

int main()
{
    std::ifstream file("examples/demo.nova");

    if(!file)
    {
        std::cout<<"Cannot open file\n";
        return 1;
    }

    std::stringstream buffer;

    buffer<<file.rdbuf();

    Lexer lexer(buffer.str());

    auto tokens=lexer.scanTokens();

    for(auto& t:tokens)
    {
        std::cout
        <<t.lexeme
        <<" "
        <<static_cast<int>(t.type)
        <<"\n";
    }

    return 0;
}