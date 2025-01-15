#ifndef VMWRITER_HPP
#define VMWRITER_HPP

using namespace std; 
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <cctype>
#include <string>
#include <vector>
#include <sstream>
#include "Parser.hpp"

class VMWriter{

    public:

    VMWriter(ostream& o) : out(o){
        
    }

    void setFileName(string f){
        filename = f;
    }
    void writeArithmetic(Parser::Command c){
       //TODO implement the boolean comparator instructions using the jumping logic, will have to keep track
       //of number of jumps in order to have unique labels, so one of the member variables will be a jump count
       //and we have to increment it to create a new unique label
       //TODO implement neg and not, should be pretty easy 
        out<<"//"<<c.instruction<<endl;
        if(arithmeticMap.find(c.type) != arithmeticMap.end()){
            /**
         * //SP -- 
            @SP
            M = M-1
            //D = M 
            A = M
            D = M
            //SP-- 
            @SP
            M = M-1
            //M = D command M 
            A = M 
            M = D command M
            //SP++ 
            @SP
            M = M+1
         */
            out<<"@SP"<<endl<<"M = M-1"<<endl<<"A = M"<<endl<<"D = M"<<endl<<"@SP"<<endl<<"M = M-1"<<endl<<"A = M"<<endl;
            out<<"M = M"<<arithmeticMap[c.type]<<"D"<<endl<<"@SP"<<endl<<"M = M+1"<<endl; 
        } else if(compMap.find(c.type) != compMap.end()){
            /**
             * @SP
                M = M-1
                A = M 
                D = M 
                @SP
                M = M-1
                A = M 
                D = M-D 
                @JumpI
                D; Command 
                M = 0
                @endI
                0;JMP
                (JumpI)
                M = 1
                (endI)
                @SP
                M = M+1
             */
            /**
             * "@SP\n" +
                "AM=M-1\n" +
                "D=M\n" +
                "A=A-1\n" +
                "D=M-D\n" +
                "@FALSE" + arthJumpFlag + "\n" +
                "D;" + type + "\n" +
                "@SP\n" +
                "A=M-1\n" +
                "M=-1\n" +
                "@CONTINUE" + arthJumpFlag + "\n" +
                "0;JMP\n" +
                "(FALSE" + arthJumpFlag + ")\n" +
                "@SP\n" +
                "A=M-1\n" +
                "M=0\n" +
                "(CONTINUE" + arthJumpFlag + ")\n";
             */
            
            out<<"@SP"<<endl<<"AM = M-1"<<endl<<"D = M"<<endl<<"@FALSE"<<jumps<<endl<<"D;"<<compMap[c.type]<<endl<<"@SP"<<endl;
            out<<"A=M-1"<<endl<<"M=-1"<<endl<<"@CONTINUE"<<jumps<<endl<<"0;JMP"<<endl<<"(FALSE"<<jumps<<")"<<endl<<"@SP"<<endl;
            out<<"A=M-1"<<endl<<"M=0"<<endl<<"(CONTINUE"<<jumps<<")"<<endl; 

        } else if(c.type == "not"){
             out<<"@SP"<<endl<<"M = M-1"<<endl<<"A = M"<<endl<<"M = !M"<<endl<<"@SP"<<endl<<"M = M+1"; 
            /**
             * @SP
                M = M-1
                A = M 
                M = !/-M 
                @SP
                M = M+1 
             */
        } else if(c.type == "neg"){
             out<<"@SP"<<endl<<"M = M-1"<<endl<<"A = M"<<endl<<"M = -M"<<endl<<"@SP"<<endl<<"M = M+1"; 
        } else {
            out<<"Unknown type"<<endl; 
        }
        
    }

    void writePushPop(Parser::Command c){
        //TODO implement push/pop pointer, rewatch video for how that works   
        out<<"//"<<c.instruction<<endl; 
        if(c.type == "pop"){
            if(symbolMap.find(c.sgmt) != symbolMap.end()){
                out<<"@"<<symbolMap[c.sgmt]<<endl;
                out<<"D = M"<<endl<<"@"<<c.index<<endl << "D = A+D"<<endl<<"@SP"<<endl<<"M = M-1"<<endl<<"@R13"<<endl<<"M = D"<<endl;
                out<<"@SP"<<endl<<"A = M"<<endl<<"D = M"<<endl<<"@R13"<<endl<<"A = M"<<endl<<"M = D"<<endl; 
                /**
                 *  @LCL
                    D = M 
                    @i 
                    D = A+D  //LCL+i 
                    @SP
                    M = M-1 //SP -- 
                    @R13
                    M = D  //storing the address 
                    @SP
                    A = M
                    D = M  //getting *SP
                    @R13
                    A = M
                    M = D //*addr = *SP;
                 */
                 
            }else if(c.sgmt == "pointer"){
                if(c.index == 0 ){
                    /**
                     * code: SP--
                        @SP
                        M = M-1
                        CODE RAM[THIS] = RAM[SP]
                        @SP
                        A = M
                        D = M
                        @THIS
                        A = M
                        M = D
                     */
                    out<<"@SP"<<endl<<"AM = M-1"<<endl<<"D = M"<<endl<<"@THIS"<<endl<<"M = D"<<endl;
                } else {
                    out<<"@SP"<<endl<<"AM = M-1"<<endl<<"D = M"<<endl<<"@THAT"<<endl<<"M = D"<<endl;
                }
            } else if(c.sgmt == "static"){
               
                out<<"@SP"<<endl<<"M = M-1"<<endl<<"A = M"<<endl<<"D = M"<<endl<<"@"<<filename<<"."<<c.index<<endl<<"M = D"<<endl;
               /**
                * @SP
                    M = M-1
                    A = M
                    D = M 
                    @Foo.i 
                    M = D 
                */

            } else if(c.sgmt == "temp"){
                out<<"@"<<5<<endl;
                out<<"D = A"<<endl<<"@"<<c.index<<endl << "D = A+D"<<endl<<"@SP"<<endl<<"M = M-1"<<endl<<"@R13"<<endl<<"M = D"<<endl;
                out<<"@SP"<<endl<<"A = M"<<endl<<"D = M"<<endl<<"@R13"<<endl<<"A = M"<<endl<<"M = D"<<endl; 
            }
        } else{
            if(symbolMap.find(c.sgmt) != symbolMap.end()){
                
                out<<"@"<<symbolMap[c.sgmt]<<endl<<"D = M"<<endl<<"@"<<c.index<<endl<<"A = A+D"<<endl<<"D = M"<<endl;
                out<<"@SP"<<endl<<"A = M"<<endl<<"M = D"<<endl<<"@SP"<<endl<<"M = M+1"<<endl; 

                /**
                 * @LCL
                    D = M 
                    @i 
                    A = A+D 
                    D = M 
                    @SP
                    A = M
                    M = D 
                    @SP
                    M = M+1 
                 */
            } else if(c.sgmt == "constant"){
                /**
                 * @i //loads 17 into A reg, D = 17
                    D = A 
                    @SP  // *SP = D 
                    A = M 
                    M = D 
                    @SP // SP++
                    M = M+1 
                 */
                out<<"@"<<c.index<<endl<<"D = A"<<endl<<"@SP"<<endl<<"A = M"<<endl<<"M = D"<<endl<<"@SP"<<endl<<"M = M+1"<<endl; 
            } else if(c.sgmt == "pointer"){
                if(c.index == 0 ){
                    /**
                     * @THIS
                        A = M
                        D = M
                        @SP
                        A = M
                        M = D

                        CODE : SP++
                        @SP
                        M = M+1
                     */
                    out<<"@THIS"<<endl<<"D = M"<<endl<<"@SP"<<endl<<"A = M"<<endl<<"M = D"<<endl<<"@SP"<<endl<<"M = M+1"<<endl;
                } else{
                    out<<"@THAT"<<endl<<"D = M"<<endl<<"@SP"<<endl<<"A = M"<<endl<<"M = D"<<endl<<"@SP"<<endl<<"M = M+1"<<endl;
                }
                
            } else if(c.sgmt == "static"){

                out<<"@"<<filename<<"."<<c.index<<endl<<"D = M"<<endl<<"@SP"<<endl<<"A = M"<<endl<<"M = D"<<endl<<"@SP"<<endl<<"M = M+1"<<endl;
                /**
                 * @Foo.i 
                    D = M 
                    @SP 
                    A = M 
                    M = D 
                    @SP
                    M = M+1
                 */
                
            } else if(c.sgmt == "temp"){
                out<<"@"<<5<<endl<<"D = A"<<endl<<"@"<<c.index<<endl<<"A = A+D"<<endl<<"D = M"<<endl;
                out<<"@SP"<<endl<<"A = M"<<endl<<"M = D"<<endl<<"@SP"<<endl<<"M = M+1"<<endl; 
            }

        }


    }

    void writeLabel(string label){
        out<<"("<<label<<")"<<endl; 

    }
    void writeGoTo(string address) {
        out<<"@"<<address<<endl<<"0;JMP"<<endl; 
    }
    void writeIf(string label) {
        /**
         * @SP 
         * AM = M-1 
         * D = M 
         * @SP
         * M = M-1 
         * @label
         * D;JNE 
         */
        string pop = 
            "@SP\n" 
            "AM = M-1\n" 
            "D = M\n" 
            "@SP\n"
            "M = M-1\n"; 
        out<<pop<<"@"<<label<<endl<<"D;JNE"<<endl; 
    }
    void writeFunction(string functionName, int numVars){
        /*
        @nArgs
            D = A 
        (FUNCTION$LOOP)
            @FUNCTION$ENDLOOP
            D;JLE 
            @SP 
            A = M 
            M = 0 
            @SP 
            M = M+1 
            D = D - 1 
            @FUNCTION$LOOP
            0;JMP 
        (FUNCTION$ENDLOOP)*/
        string endloop = functionName+"$LOOP";
        string middlePart = 
            "D;JLE\n" 
            "@SP\n" 
            "A = M\n" 
            "M = 0\n" 
            "@SP\n" 
            "M = M+1\n" 
            "D = D - 1\n"; 
        out<<"@"<<numVars<<endl<<"D = A"<<endl<<"("<<endloop<<")"<<endl<<"@"<<endloop<<endl<<middlePart<<"@"<<endloop<<endl<<"0;JMP"<<endl;

    }
    void writeCall(string functionName, int numArgs){
        string returnAddress = functionName+"$ret."+to_string(returns); 
        string pushTemplate  = 
        "@SP\n"
        "A = M\n"
        "M = D\n"
        "@SP\n"
        "M = M+1\n"; 
        /**
         *  @Foo$ret.1
            D = A 
            @SP
            A = M 
            M = D 
            @SP 
            M = M+1 
         */
        out<<"@"<<returnAddress<<endl<<"D = A"<<endl<<pushTemplate<<endl;     
        vector<string> segments = {"LCL", "ARG", "THIS","THAT"};
        for(string sg : segments){
            out<<"@"<<sg<<endl<<"D=M"<<endl<<pushTemplate<<endl; 
        }
        /**
         *  @SP
            D = A 
            @5
            D = D-A 
            @nArgs
            D = D-A
         */
        out<<"@SP"<<endl<<"D = A"<<endl<<"@5"<<endl<<"D = D-A"<<endl<<"@"<<numArgs<<endl<<"D = D-A"<<endl; 
        /**
         * @SP
            D = A 
            @LCL
            M = D 
         */
        out<<"@SP"<<endl<<"D = A"<<endl<<"@LCL"<<endl<<"M = D"<<endl; 
        out<<"@"<<functionName<<endl<<"0;jmp"<<endl<<"("<<returnAddress<<")"<<endl; 

        returns++; 
    }
    void writeReturn() {
        string endFrame = 
            "@LCL\n"
            "D = M\n"
            "@R13\n"
            "M = D\n"; 
        out<<endFrame; 
        string retAddrSeg = 
            "@5\n"
            "D = D-A\n"
            "A = D\n" 
            "D = M\n"
            "@R14\n" 
            "M = D\n" ; 
        out<<retAddrSeg; 
        string argPop  = 
            "@SP\n"
            "AM = M-1\n"
            "D = M\n" 
            "@ARG\n" 
            "M = D\n";
        out<<argPop; 
        string incSP = 
            "@ARG\n"
            "D = A + 1\n" 
            "@SP\n"
            "M = D\n"; 
        out<<incSP; 
        string assignThat = 
            "@R13\n"
            "D = M-1\n" 
            "A = D\n" 
            "D = M\n" 
            "@THAT\n"
            "M = D\n";
        out<<assignThat; 
        vector<string> otherSegments = {"THIS", "ARG","LCL"}; 
        for(int i = 2; i <= 4; i++){
            out<<"@R13"<<endl<<"D=M"<<endl<<"@"<<i<<endl<<"D = D-A"<<endl<<"A = D"<<endl<<"D = M"<<endl<<"@"<<otherSegments[i-1]<<endl<<"M = D";
        }
        out << "@R14\n"
       "A = M\n"
       "0;JMP\n"; 
        
    }
    void writeInit(){
        string bootStrap = "@256\n"
                         "D=A\n"
                         "@SP\n"
                         "M=D\n";
        out<<bootStrap; 
        writeCall("Sys.init",0);
    }
    void write(Parser::Command c){
        if(c.type == "push" || c.type == "pop"){
            writePushPop(c);
        } else if(c.type == "label"){
            writeLabel(c.sgmt);
        } else if(c.type == "goto"){
            writeGoTo(c.sgmt);
        } else if(c.type == "if-goto"){
            writeIf(c.sgmt);
        } else if(c.type == "function"){
            writeFunction(c.sgmt,c.index);
        } else if(c.type == "call"){
            writeCall(c.sgmt,c.index);
        } else if(c.type == "return"){
            writeReturn();
        }else {
            writeArithmetic(c); 
        }
    }



    private: 
    unsigned int jumps = 0; 
    unsigned int returns; 
    string filename; 
    ostream& out; 
    map<string, string> symbolMap = {
            {"local", "LCL"},
            {"argument", "ARG"},
            {"this", "THIS"},
            {"that", "THAT"},
        };
    map<string, string> arithmeticMap = {
            {"add", "+"},
            {"sub", "-"},
            {"or", "|"},
            {"and", "&"}
        };
    
    map<string, string> compMap = {
            {"eq", "JNE"},
            {"gt", "JLE"},
            {"lt", "JGE"},
        };


};


#endif