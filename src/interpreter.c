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

void bf_execute(bf_lexer_t *lexer)
{
	stack_t *stack = init_stack();
	memory_t *current_memory_cell = lexer->memory_head;

	for(int pc = 0; lexer->file_buffer[pc] != '\0'; ++pc)
	{
		char curr_token = lexer->file_buffer[pc];
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
						 if(!current_memory_cell->value)
						 {
							 int nested_presence = 0;

							 while(nested_presence)
							 {
								 switch(lexer->file_buffer[pc++])
								 {
									 case '[': ++nested_presence; break;
									 case ']': --nested_presence; break;
								 }
							 }
						 }
						 else
							 stack_push(stack, pc);

						 break;
			case R_BRAC:
						 if(current_memory_cell->value)
							pc = peek_stack(stack);
						 else
							 stack_pop(stack);
						 break;
		}
	}
	putchar('\n');

	free_list(current_memory_cell);
	free_stack(stack);
}

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
	stack->stack = calloc(30, sizeof(int));
	stack->length = 30;
	stack->sp = -1;
	return (stack);
}

int peek_stack(stack_t *stack)
{
	return (stack->stack[stack->sp]);
}

int is_empty_stack(stack_t *stack)
{
	return (stack->sp < 0);
}

void stack_push(stack_t *stack, int value)
{
	stack->capacity++;

	if(stack->length == stack->capacity)
	{
		int updated_length = stack->length * 2;
		stack->stack = realloc(stack->stack, updated_length);
	}

	stack->sp++;
	stack->stack[stack->sp] = value;
}

void stack_pop(stack_t *stack)
{
	LASSERT(!is_empty_stack(stack), "Stack underflow! Stack is empty\n");
	stack->sp--;
}

void free_stack(stack_t *stack)
{
	free(stack->stack);
	free(stack);
}

