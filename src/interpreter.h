#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#ifdef DEBUG
#include <malloc.h>

#define WHI  "\x1b[1;37m"
#define RED  "\x1b[1;31m"
#define GRE  "\x1b[1;32m"
#define BLUE "\x1b[1;34m"
#define PURP "\x1b[1;35m"
#endif

#define LASSERT(condition, message, ...) \
    do \
    { \
        if(!(condition)) \
        { \
            fputs("[*] Process terminated! LASSERT(); failed!", stderr); \
            fprintf(stderr, "\nCondition\t\t: {%s}\nFunction\t\t: {%s}\nFailed in file\t\t: {%s}\nAt line \t\t: {%d}\n", #condition, __func__, __FILE__, __LINE__);\
            fprintf(stderr, message, ##__VA_ARGS__); \
            exit(1); \
        } \
    } while(0); \

enum token_types {
	INC		 = '+', // *(buffer_ptr++);
	DEC		 = '-', // *(buffer_ptr--);
	INC_DP   = '>', // buffer_ptr++;
	DEC_DP   = '<', // buffer_ptr--;
	PRINT	 = '.', // putchar(*buffer_ptr);
	READ	 = ',', // getchar(*buffer_ptr);
	L_BRAC   = '[', // while(*buffer_ptr) {
	R_BRAC   = ']'  // }
};

// lexer structure:
struct lexer {
	// Lexer aspects:
	char *file_buffer;
	int file_buffer_size;

	// Dynamic array:
	char *token_stream;
	size_t token_stream_length, token_stream_capacity;
} typedef bf_lexer_t;

// Interpreter:
void bf_compile(void);
void bf_execute(void);

// Dynamic array handlers:
void lexer_push_token(char);

#ifdef DEBUG
void display_token_stream(void);
#endif

// Lexer:
void init_lexer(FILE *);
void destroy_lexer(void);

// Misc:
int set_file_size(FILE *);

#endif

