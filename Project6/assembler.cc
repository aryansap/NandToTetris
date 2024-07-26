/* initialize shitters like variable map and 2d array
/*7/9 -- figure out how to create main function, read in file and initialize array and map.
*/
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <cctype>
#include <string>
#include <ranges>
std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r");
    size_t end = str.find_last_not_of(" \t\r");
    return (start == std::string::npos || end == std::string::npos) ? "" : str.substr(start, end - start + 1);
}
std::map<std::string,std::string> initializeSymbol(){
    std::map<std::string, std::string> symbolmap;
    for (int i = 0; i <= 15; ++i) {
        symbolmap["R" + std::to_string(i)] = std::bitset<15>(i).to_string();
    }
    symbolmap["SCREEN"] = std::bitset<15>(16384).to_string();
    symbolmap["KBD"] = std::bitset<15>(24576).to_string();
    symbolmap["SP"] = std::bitset<15>(0).to_string();
    symbolmap["LCL"] = std::bitset<15>(1).to_string();
    symbolmap["ARG"] = std::bitset<15>(2).to_string();
    symbolmap["THIS"] = std::bitset<15>(3).to_string();
    symbolmap["THAT"] = std::bitset<15>(4).to_string();

    return symbolmap;
}

//create a instruction handling here
std::string aInstruction(std::string line, unsigned int& memorycounter, std::map<std::string,std::string>& varmap){
    std::string ret = "0"; 
    unsigned int atindex = line.find("@");
    std::string num = line.substr(atindex+1);
    num = trim(num);
    if (isdigit(num.at(0))){
        std::bitset<15> val(std::stoi(num));
        return ret + ""+val.to_string();
    } else {
        if(varmap.find(num) == varmap.end()){
            std::bitset<15> newvar(memorycounter++);
            varmap[num] = newvar.to_string();
        }
        return ret + ""+ varmap[num];
    }
}
//AS OF NOW AINSTRUCTION method is done and mostly tested, now we have to deal with c instruction and first pass

//c instruction handling here
std::string cInstruction(const std::string& line){
    std::string initial = "111";
    std::string dest = "000";
    std::string comp = "0000000";
    std::string jump = "000";
    std::map<std::string,std::string> compmap={{"0","0101010"},{"1","0111111"},{"-1","0111010"},{"D","0001100"},{"A","0110000"},{"M","1110000"},{"!D","0001101"}, {"!A","0110001"},
    {"!M","1110001"},{"-D","0001111"},{"-A","0110011"},{"-M","1110011"},{"D+1","0011111"},{"A+1","01101110"},{"M+1","11101110"},{"D-1","0001110"},
        {"A-1","0110010"},{"M-1","1110010"}, {"D+A","0000010"},{"D+M","1000010"}, {"D-A","0010011"},{"D-M","1010011"},{"A-D","0000111"},
        {"M-D","1000111"},{"D&A","0000000"},{"D&M","1000000"}, {"D|A","0010101"},{"D|M","1010101"} };
    //some logic here to determine each bit, we have to isolate each part
    unsigned int charpointer = 0;
    if (line.find('=') != std::string::npos) {
        for (unsigned int i = 0; i < line.find('='); ++i) {
            if (line.at(charpointer) == 'A') {
                dest.at(0) = '1';
            }
            if (line.at(charpointer) == 'D') {
                dest.at(1) = '1';
            }
            if (line.at(charpointer) == 'M') {
                dest.at(2) = '1';
            }
            charpointer++;
        }
        charpointer++;
    }

    unsigned int endofcomp = line.size();
    if (line.find(';') != std::string::npos) {
        endofcomp = line.find(';');
    }

    std::string compa = "";
    for (unsigned int i = charpointer; i < endofcomp; ++i) {
        if (line.at(i) != ' ' && line.at(i) != '='&& line.at(i) != '\r'){
            compa += line.at(i);
        }
    }

    if (compmap.find(compa) != compmap.end()) {
        comp = compmap[compa];
    } else {
        std::cout<< "Error: Unknown comp field: " << compa <<std::endl <<std::flush;
        
    }
  

    if (line.find("JGT") != std::string::npos) {
        jump = "001";
    } else if (line.find("JEQ") != std::string::npos) {
        jump = "010";
    } else if (line.find("JGE") != std::string::npos) {
        jump = "011";
    } else if (line.find("JLT") != std::string::npos) {
        jump = "100";
    } else if (line.find("JNE") != std::string::npos) {
        jump = "101";
    } else if (line.find("JLE") != std::string::npos) {
        jump = "110";
    } else if (line.find("JMP") != std::string::npos) {
        jump = "111";
    }

    return initial + comp + dest + jump;
}



int main(int argc, char* argv[]) { 
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input file>" << std::endl;
        return 1;
    }

    std::ifstream ifs(argv[1]); // Correctly use argv[1] to open the file
    if (!ifs) {
        std::cerr << "Error opening file: " << argv[1] << std::endl;
        return 1;
    }
    std::map<std::string,std::string> symbolmap;
    symbolmap = initializeSymbol();
    
    unsigned int memorycounter = 16; //first available memory unit for variables
    std::vector<std::string> assemblycode;
    std::vector<std::vector<std::string> > newcode;
    std::string line;
    int counter = 0;
     while (std::getline(ifs, line)) { 
        line = trim(line); // Trim the line to remove leading and trailing spaces
        if (line.empty() || line[0] == '/') { // Skip empty lines and comments
            continue;
        }
        assemblycode.push_back(line);
     }
   
    
    //first pass
    for(std::string line : assemblycode){
        std::string var = "";
        if(line.find('(') != std::string::npos){
            std::cout<<"reached here!"<<std::endl;
            int i = line.find('(')+1;
            while(line.at(i) != ')'){
                var+=line.at(i);
                i++;
            }
            counter--;
            
            symbolmap[var] = std::bitset<15>(counter+1).to_string();
        }
        counter++;
    }

    counter = 0;
    //second pass
    std::string inputfile = argv[1];
    size_t lastDot = inputfile.find_last_of('.');
    std::string baseName = inputfile.substr(0, lastDot);
    std::string outputFile = baseName + ".hack";
    std::ofstream out(outputFile);
     for(std::string line: assemblycode) { 
 
        if(line.find('@') != std::string::npos){
            out<<aInstruction(line,memorycounter,symbolmap)<<std::endl;
        }
        if(line.find('=') != std::string::npos || line.find(';') != std::string::npos){
            out<<cInstruction(line)<<std::endl;
        }
        counter++;
    } 
    out.close();


    return 0;
 }