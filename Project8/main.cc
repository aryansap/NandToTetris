#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem> // C++17 feature for handling directories
#include "Parser.hpp"
#include "VMWriter.hpp"
namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input file or directory>" << std::endl;
        return 1;
    }

    std::string inputPath = argv[1];
    std::ofstream out;
    if (fs::is_directory(inputPath) && fs::directory_iterator(inputPath) == fs::end(fs::directory_iterator())) {
        std::cerr << "Directory is empty or contains no .vm files." << std::endl;
        return 1;
    }

    if (fs::is_directory(inputPath)) {
        // If input is a directory
        std::string baseName = fs::path(inputPath).filename().string();
        std::string outputFile = inputPath + "/" + baseName + ".asm";
        out.open(outputFile);

        if (!out.is_open()) {
            std::cerr << "Failed to open output file: " << outputFile << std::endl;
            return 1;
        }

        VMWriter writer(out);
        writer.writeInit();
        for (const auto& entry : fs::directory_iterator(inputPath)) {
            if (entry.path().extension() == ".vm") {
                std::ifstream ifs(entry.path());
                if (!ifs.is_open()) {
                    std::cerr << "Failed to open input file: " << entry.path() << std::endl;
                    continue;
                }
                std::string fileNameWithoutExtension = entry.path().stem().string();
                writer.setFileName(fileNameWithoutExtension);
                Parser parser(ifs);
                std::vector<Parser::Command> commands = parser.getCommands();
                for (const auto& command : commands) {
                    writer.write(command);
                }
            }
        }
    } else if (fs::is_regular_file(inputPath)) {
        // If input is a single file
        size_t lastDot = inputPath.find_last_of('.');
        std::string baseName = inputPath.substr(0, lastDot);
        std::string outputFile = baseName + ".asm";
        out.open(outputFile);
        
        if (!out.is_open()) {
            std::cerr << "Failed to open output file: " << outputFile << std::endl;
            return 1;
        }

        std::ifstream ifs(inputPath);
        if (!ifs.is_open()) {
            std::cerr << "Failed to open input file: " << inputPath << std::endl;
            return 1;
        }

        Parser parser(ifs);
        VMWriter writer(out);
        writer.setFileName(baseName);
        std::vector<Parser::Command> commands = parser.getCommands();
        for (const auto& command : commands) {
            writer.write(command);
        }
    } else {
        std::cerr << "Invalid input path: " << inputPath << std::endl;
        return 1;
    }

    return 0;
}
