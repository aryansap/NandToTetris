#ifndef PARSER_HPP
#define PARSER_HPP

// Your declarations and definitions here


using namespace std; 
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <cctype>
#include <string>
#include <vector>
#include <sstream>

std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r");
    size_t end = str.find_last_not_of(" \t\r");
    return (start == std::string::npos || end == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

std::vector<std::string> splitString(const std::string& str) {
    std::istringstream iss(str);
    std::string word;
    std::vector<std::string> result;

    while (std::getline(iss, word, ' ')) { // Split by space
        result.push_back(word);
    }

    return result;
}
class Parser {
    public: 
    struct Command{
        string instruction; // this way we can print out the instructions for debugging 
        string type; 
        string sgmt;
        int index = -1; 
    };

    
    
    Parser(ifstream& input) : ifs(input) {
        if (!ifs) {
            throw std::invalid_argument("Invalid file stream passed to Parser.");
        }
    }

    vector<Command> getCommands() {
        commands.clear(); 
        string line;
        while (std::getline(ifs, line)) { 
            line = trim(line); // Trim the line to remove leading and trailing spaces

            // Remove inline comments
            size_t commentPos = line.find("//");
            if (commentPos != std::string::npos) {
                line = line.substr(0, commentPos); // Keep only the part before "//"
                line = trim(line); // Trim again to remove any trailing spaces
            }

            // Skip empty lines after removing comments
            if (line.empty()) {
                continue;
            }

            // Split the line into components
            std::vector<string> splitLine = splitString(line);
            Command curr;
            curr.instruction = line; 
            curr.type = splitLine[0];

            // Handle cases where the command has 3 parts (e.g., "push segment index")
            if (splitLine.size() == 3) {
                curr.sgmt = splitLine[1];
                curr.index = stoi(splitLine[2]);
            }

            commands.push_back(curr);
        }
        return commands;
    }


    private: 
    ifstream& ifs; 
    vector<Command> commands; 

};


#endif // YOUR_HEADER_FILE_NAME_HPP