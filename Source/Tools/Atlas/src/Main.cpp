#include "Commands.h"

#include <CliParser/CliParser.h>
#include <iostream>

int main(int argc, char** argv) {
    InitializeCommands();
    CliParser::Run(argc, argv, std::cerr);
}