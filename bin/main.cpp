#include <functional>
#include <lib/ArgParser.h>

#include <iostream>
#include <numeric>

struct Options {
    bool sum = false;
    bool mult = false;
};

int main(int argc, char** argv) {
    Options opt;
    std::vector<int> values;

    ArgumentParser::ArgParser parser("Program");
    parser.AddHelp('h', "help", "Program accumulate arguments");

    if(!parser.Parse(argc, argv)) {
        std::cout << "Wrong argument" << std::endl;
        std::cout << parser.HelpDescription() << std::endl;
        return 1;
    }

    return 0;

}
