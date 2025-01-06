#include "Parser.hpp"
#include "VMWriter.hpp"
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <cctype>
#include <string>
#include <ranges>
int main(int argc, char* argv[]) { 
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input file>" << std::endl;
        return 1;
    }
  

    std::ifstream ifs(argv[1]);
    Parser parser(ifs); 
    std::string inputfile = argv[1];
    size_t lastDot = inputfile.find_last_of('.');
    std::string baseName = inputfile.substr(0, lastDot);
    std::string outputFile = baseName + ".asm";
    std::ofstream out(outputFile);
    
    std::vector<Parser::Command> commands = parser.getCommands();
    VMWriter writer(baseName, out); 
    for(Parser::Command c: commands){
        writer.write(c);
    }

    return 0;
}