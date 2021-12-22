Instructions:To run our program, make sure you are in the project directory. Type make clean, enter, type make, enter. 
This will set up the simulator file to be run. You can then enter for example ./sim -i tests/class.mc
That will run the program with the supplied instructions. You can interchange class.mc with any other test we have
provided inside the tests folder. 

Files Included:
Makefile: helps run the program by preloading all of the gcc commands into assembler so all you have to type is ./sim to run

sim.c: This is the c code we have written to run machine code and print the output to the command line. It will show the state of
each register and memory location after operations are ran from the machine code using input from our tests. 

tests/
class.mc: This is the supplied machine code file that is used to test correctness our simulator output. 

test1: tests to see if jalr, sw, lw, noop, add, done works

test2: tests to see if can jump to a memory location outside of allocated memory 

multiMc: tests if the multiplication function works
