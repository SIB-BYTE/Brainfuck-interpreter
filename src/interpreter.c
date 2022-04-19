#include "interpreter.h"

#ifdef DEBUG
void __attribute__((constructor)) setup(void)
{
	printf("%s==============DEBUG=========================\n\n", WHI);
}

void __attribute__((destructor)) destroy(void)
{
	printf("%s\n=============End============================\n", WHI);
}
#endif

void bf_compile(bf_lexer_t *lexer)
{
	LASSERT(lexer, "Lexer is NULL\n");

	for(int pc = 0; pc < lexer->file_buffer_size; ++pc)
	{
		switch(lexer->file_buffer[pc])
		{
			case INC:    lexer_push_token(lexer, INC);     break;
			case DEC:    lexer_push_token(lexer, DEC);     break;
			case INC_DP: lexer_push_token(lexer, INC_DP);  break;
			case DEC_DP: lexer_push_token(lexer, DEC_DP);  break;
			case PRINT:  lexer_push_token(lexer, PRINT);   break;
			case READ:   lexer_push_token(lexer, READ);    break;
			case L_BRAC: lexer_push_token(lexer, L_BRAC);  break;
			case R_BRAC: lexer_push_token(lexer, R_BRAC);  break;
			default: pc++; break;
		}
	}
#ifdef DEBUG
	printf("%sToken stream malloc'd size: %s%ld bytes\n\n", BLUE, PURP, malloc_usable_size(lexer->token_stream));
#endif

	lexer->token_stream[lexer->token_stream_length] = '\0';
}

void bf_execute(bf_lexer_t *lexer)
{
	memory_t *current_memory_cell = lexer->memory_head;
	stack_t *stack = init_stack();

	for(int pc = 0; lexer->token_stream[pc] != '\0'; ++pc)
	{
		char curr_token = lexer->token_stream[pc];
#ifdef DEBUG
		printf("pc => {%d} | Token = {%c} |  Mem => {%d}\n", pc, curr_token, current_memory_cell->value);
#endif

		switch(curr_token)
		{
			case INC:    ++(current_memory_cell->value); break;
			case DEC:    --(current_memory_cell->value); break;
			case INC_DP: current_memory_cell = next_cell(current_memory_cell); break;
			case DEC_DP: current_memory_cell = prev_cell(current_memory_cell); break;
			case PRINT:  putchar(current_memory_cell->value); fflush(stdout);  break;
			case READ:   current_memory_cell->value = getchar();  break;
			case L_BRAC:
					     if(current_memory_cell->value)
						 {
#ifdef DEBUG
							 printf("Push at pc -> {%d}\n", pc);
#endif
							 push(stack, pc);
						 }
						 else
						 {
							 int loop_presence = 1;

							 while(loop_presence)
							 {
								 switch(lexer->token_stream[++pc])
								 {
									 case '[': ++loop_presence; break;
									 case ']': --loop_presence; break;
								 }
							 }
							 pc++;
#ifdef DEBUG
							  printf("loop_presence => {%s%d}\n", RED, loop_presence);
#endif
						 }
						 break;

			case R_BRAC:
#ifdef DEBUG
						 printf("Pop at pc -> {%d}\n", pc);
#endif
						 if(!current_memory_cell->value)
							 pc = peek(stack);
						 else
							 pop(stack);

						 break;
		}
	}
	putchar('\n');

	free_list(current_memory_cell);
	free_stack(stack);
}

void lexer_push_token(bf_lexer_t *lexer, char token)
{
	LASSERT(lexer, "lexer is NULL\n");

	if(lexer->token_stream_length == lexer->token_stream_capacity)
	{
		lexer->token_stream_capacity = lexer->token_stream_capacity * 2 + 1;
		char *new_token_stream = realloc(lexer->token_stream, lexer->token_stream_capacity);

		LASSERT(new_token_stream, "Memory allocation failure!\n");

		lexer->token_stream = new_token_stream;
	}

	lexer->token_stream[lexer->token_stream_length] = token;
	lexer->token_stream_length++;
}

#ifdef DEBUG
void display_token_stream(bf_lexer_t *lexer)
{
	LASSERT(lexer && lexer->token_stream, "Lexer and lexer->token_stream() are NULL\n");

	printf("%sToken stream debug data: \n", WHI);
	for(size_t i = 0; i < lexer->token_stream_length; ++i)
		printf("%sToken stream[%lu] => {%c}\n", RED, i, lexer->token_stream[i]);
}
#endif

bf_lexer_t *init_lexer(FILE *fd)
{
	bf_lexer_t *lexer = calloc(1, sizeof(bf_lexer_t));
	LASSERT(lexer, "lexer allocation failed!\n");

	lexer->memory_head = init_memory();

	fseek(fd, 0, SEEK_END);
	int size = ftell(fd);
	rewind(fd);

	lexer->file_buffer_size = size;
	lexer->file_buffer = malloc(size + 1);
	lexer->file_buffer[size] = '\0';

#ifdef DEBUG
	puts("File && memory allocation debugging: ");
	printf("%sFile size         : %s%d bytes\n", BLUE, PURP, lexer->file_buffer_size);
	printf("%sMalloc'd file size: %s%ld bytes\n", BLUE, PURP, malloc_usable_size(lexer->file_buffer));
#endif

	LASSERT(fread(lexer->file_buffer, 1, size, fd) == (size_t) size, "fread() error!\n");

#ifdef DEBUG
	printf("File_buffer[%d] => %s\n", size, lexer->file_buffer);
#endif

	return (lexer);
}

void destroy_lexer(bf_lexer_t *lexer)
{
	free(lexer->token_stream);
	free(lexer->file_buffer);
	free(lexer);
}

// Linked list functions:
memory_t *init_memory(void)
{
	memory_t *memory = calloc(1, sizeof(memory_t));
	return (memory);
}

memory_t *next_cell(memory_t *cell)
{
	if(cell->fd == NULL)
		cell->fd = init_memory();

	cell->fd->bk = cell;
	cell = cell->fd;

	return (cell);
}

memory_t *prev_cell(memory_t *cell)
{
	if(cell->bk == NULL)
		cell->bk = init_memory();

	cell->bk->fd = cell;
	cell = cell->bk;

	return (cell);
}

void free_list(memory_t *memory)
{
	memory_t *tmp = 0;

	while(memory->bk)
		memory = memory->bk;

	while(memory)
	{
		tmp = memory->fd;
		free(memory);
		memory = tmp;
	}
}

// Stack functions:
stack_t *init_stack(void)
{
	stack_t *stack = calloc(1, sizeof(stack_t));
	stack->sp = -1;
	return (stack);
}

int peek(stack_t *stack)
{
	return (stack->bracket_stack[stack->sp]);
}

int is_full(stack_t *stack)
{
	return (stack->sp > 25);
}

int is_empty(stack_t *stack)
{
	return (stack->sp < 0);
}

void push(stack_t *stack, int value)
{
	LASSERT(!is_full(stack), "Stack overflow! Stack is full\n");
	stack->sp++;
	stack->bracket_stack[stack->sp] = value;
}

void pop(stack_t *stack)
{
	LASSERT(!is_empty(stack), "Stack underflow! Stack is empty\n");
	stack->sp--;
}

void free_stack(stack_t *stack)
{
	free(stack);
}

