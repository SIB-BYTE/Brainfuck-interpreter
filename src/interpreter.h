#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
	INC	 = '+', // *(buffer_ptr++);
	DEC      = '-', // *(buffer_ptr--);
	INC_DP   = '>', // buffer_ptr++;
	DEC_DP   = '<', // buffer_ptr--;
	PRINT	 = '.', // putchar(*buffer_ptr);
	READ	 = ',', // getchar(*buffer_ptr);
	L_BRAC   = '[', // while(*buffer_ptr) {
	R_BRAC   = ']'  // }
};

// Linked list:
typedef struct memory {
	char value;
	struct memory *fd, *bk;
} memory_t;

// lexer structure:
typedef struct lexer {
	// Lexer aspects:
	char *file_buffer;
	int file_buffer_size;

	memory_t *memory_head;
} bf_lexer_t;

// Stack structure:
typedef struct stack_t {
	int *stack, sp, length, capacity;
} stack_t;

// Interpreter:
void bf_execute(bf_lexer_t *);

// Debugged functions:
#ifdef DEBUG
void display_token_stream(bf_lexer_t *);
#endif

// Lexer:
bf_lexer_t *init_lexer(FILE *);
void destroy_lexer(bf_lexer_t *);

// Linked list functions:
memory_t *init_memory(void);
memory_t *next_cell(memory_t *);
memory_t *prev_cell(memory_t *);
void free_list(memory_t *);

// Stack functions:
stack_t *init_stack(void);

// Functionalities:
void stack_pop(stack_t *);
void stack_push(stack_t *, int);
void free_stack(stack_t *);

// Misc:
int peek_stack(stack_t *);
int is_empty_stack(stack_t *);

#endif
