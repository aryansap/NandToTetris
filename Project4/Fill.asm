// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/4/Fill.asm

// Runs an infinite loop that listens to the keyboard input. 
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel. When no key is pressed, 
// the screen should be cleared.

//// Replace this comment with your code.
//PSeudocode
//check kbd and if its 0 go to white loop
//else go to black loop
//at the end go back to the top of the program 

(LOOP)
    @KBD
    D=M

    @WHITE
    D;JEQ

    (BLACK)
        @SCREEN
        D=M
        @i
        M=D
        @8100
        D=A
        @n
        M=D
        (BLACKLOOP)
            @i
            D=M
            @n
            D=D-M
            @LOOP
            D;JEQ

            @i
            M=M+1
            D=M
            @SCREEN
            A=A+D
            M=-1
            @BLACKLOOP
            0;JMP
    

    (WHITE)
        @SCREEN
        D=M
        @i
        M=D
        @8100
        D=A
        @n
        M=D
        (WHITELOOP)
            @i
            D=M
            @n
            D=D-M
            @LOOP
            D;JEQ

            @i
            M=M+1
            D=M
            @SCREEN
            A=D+A
            M=0
            @WHITELOOP
            0;JMP


    