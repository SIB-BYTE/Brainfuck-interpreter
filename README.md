# Brainfuck-interpreter
Trivial interpreter for brainfuck written in Rust.

# Preface
I want to preface by saying this does not include any sort of lexer, parser or structured representation of a brainfuck application due to it's extremely trivial syntax. So just keep that in mind. Also this brainfuck implementation will not allow moving before the start of the first cell or expand the tape upon the reaching of the last cell.

# What is brainfuck?
Brainfuck is amongst the esoteric & turing tarpit families of programming languages. It is turing complete, but is not ideal for generalized development which makes it a turing tarpit, but theoretically it could represent any application any other turing complete language can. It's consistent of 8 commands that you can operate on a tape (array of char cells) that's of length 30,000 bytes wide with such. The representation of brainfuck in C would look like:
```c
char tape[30000] = {0};
char *ptr = tape;
```
Yes this is it. The commands are defined below:

# Layout:
| Command  | Action 
|--------- | ------ 
| >        | Shifts ``ptr`` once to the right.                       |
| <        | Shifts ``ptr`` once to the left.                        |
| +        | Adds 1 to the value of ``*ptr``.                        |
| -        | Subtracts 1 from the value of ``*ptr``.                 |
| .        | Prints the current ASCII byte at ``*ptr``               |
| ,        | Reads a byte from stdin and stores it at ``*ptr``       |
| [        | Opens up a loop and iterates until ``*ptr`` is 0.       |
| ]        | Closes up the loop and iterates while ``*ptr`` isn't 0. |

# Resources:
https://en.wikipedia.org/wiki/Brainfuck#Language_design

