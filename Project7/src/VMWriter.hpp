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

    VMWriter(string f, ostream& o) : filename(f), out(o){
        
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
            out<<"M = D"<<arithmeticMap[c.type]<<"M"<<endl<<"@SP"<<endl<<"M = M+1"<<endl; 
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
            out<<"@SP"<<endl<<"M = M-1"<<endl<<"A = M"<<endl<<"D = M"<<endl<<"@SP"<<endl<<"M = M-1"<<endl<<"A = M"<<endl<<"D = M-D"<<endl;
            out<<"@Jump"<<jumps<<endl<<"D;"<<compMap[c.type]<<endl<<"M = 0"<<endl<<"@end"<<jumps<<endl<<"0;JMP"<<endl<<"(Jump"<<jumps<<")"<<endl;
            out<<"M = 1"<<endl<<"(end"<<jumps<<")"<<endl<<"@SP"<<"M = M+1"<<endl; 

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
                    out<<"@SP"<<endl<<"M = M-1"<<endl<<"A=M"<<endl<<"D = M"<<endl<<"@THIS"<<endl<<"A = M"<<endl<<"M = D"<<endl;
                } else {
                    out<<"@SP"<<endl<<"M = M-1"<<endl<<"A=M"<<endl<<"D = M"<<endl<<"@THAT"<<endl<<"A = M"<<endl<<"M = D"<<endl;
                }
                /**
                 * 
                 */
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
                    out<<"@THIS"<<endl<<"A = M"<<endl<<"D = M"<<endl<<"@SP"<<endl<<"A = M"<<endl<<"M = D"<<endl<<"@SP"<<endl<<"M = M+1"<<endl;
                } else{
                    out<<"@THAT"<<endl<<"A = M"<<endl<<"D = M"<<endl<<"@SP"<<endl<<"A = M"<<endl<<"M = D"<<endl<<"@SP"<<endl<<"M = M+1"<<endl;
                }
                /**
                 * 
                 */
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
                
            }

        }


    }

    void write(Parser::Command c){
        if(c.type == "push" || c.type == "pop"){
            writePushPop(c);
        } else {
            writeArithmetic(c); 
        }
    }



    private: 
    unsigned int jumps = 0; 
    string filename; 
    ostream& out; 
    map<string, string> symbolMap = {
            {"local", "LCL"},
            {"argument", "ARG"},
            {"this", "THIS"},
            {"that", "THAT"},
            {"temp", "5"}
        };
    map<string, string> arithmeticMap = {
            {"add", "+"},
            {"sub", "-"},
            {"or", "|"},
            {"and", "&"}
        };
    
    map<string, string> compMap = {
            {"eq", "JEQ"},
            {"gt", "JGT"},
            {"lt", "JLT"},
        };


};


#endif