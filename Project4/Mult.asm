// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/4/Mult.asm

// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)
// The algorithm is based on repetitive addition.

//// Replace this comment with your code.

///psuedo code
//set sum = 0
//set i = 0
//if i > R1 go to end
//inside loop keep adding r0 to sum

@R1 
D = M 
@n 
M = D  //n is our stopping condition and we are makign that R1 
@i 
M = 1
@sum 
M = 0

(LOOP)
@i 
D = M 
@n 
D = D - M 
@STOP 
D; JGT //if i > n then goto stop 

@sum
D = M 
@R0
D = D + M // we've assigned d+r0 to the d placeholder, now we have to reassign to sum
@sum 
M = D 
@i 
M = M + 1 //i = i + 1
@LOOP
0;JMP

(STOP)
@sum
D = M 
@R2
M = D ///reassign r2 to sum 

(END)
@END
0;JMP






