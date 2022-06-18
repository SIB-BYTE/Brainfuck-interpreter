#include "interpreter.h"

/*
 * Data structure layout explanation:
 
 Linked list: The usage of the linked list is to generate the cells of the brainfuck "array", the reason for a linked list and well, not an array is
 because it makes it extremely easy to insert nodes behind the head and at the end of the 30000 cells you have access too.
 
 Stack: Makes handling the control flow operations (the '[' and the ']) is very easy by just pushing the index of "lexer->file_buffer" onto the stack and just
 constantly peek the stack over and over again until the loops ending character reaches a value beyond 0 which is how brainfuck is turing complete and can
 preform looping.
*/

// Some good look shit, really useless bad idea.
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

// Executes each individual byte we read.
void bf_execute(bf_lexer_t *lexer)
{
	// Stack structure initialization & Memory linked list:
	stack_t *stack = init_stack();
	memory_t *current_memory_cell = lexer->memory_head;

	// Iterate over bytes of the file buffer that's populated with what we read from the file.
	for(int pc = 0; lexer->file_buffer[pc] != '\0'; ++pc)
	{
		// Current token we're looking at:
		char curr_token = lexer->file_buffer[pc];
#ifdef DEBUG
		printf("pc => {%d} | Token = {%c} |  Mem => {%d}\n", pc, curr_token, current_memory_cell->value);
#endif

		switch(curr_token)
		{
			// Increments current cell value:
			case INC:    ++(current_memory_cell->value); break;
			// Decrements current cell value:
			case DEC:    --(current_memory_cell->value); break;
			// Increases the current cells pointer:
			case INC_DP: current_memory_cell = next_cell(current_memory_cell); break;
			// Decreases the current cells pointer:
			case DEC_DP: current_memory_cell = prev_cell(current_memory_cell); break;
			// Prints the current cell value:
			case PRINT:  putchar(current_memory_cell->value); fflush(stdout);  break;
			// Read from stdin and store it in the current cell:
			case READ:   current_memory_cell->value = getchar();  break;
			// Opening loop lexme:
			case L_BRAC:
						 if(!current_memory_cell->value)
						 {
							 int nested_presence = 0;

							 // Handles nested layers of loops:
							 while(!nested_presence)
							 {
								 // Checks for the next character in the file_buffer
								 switch(lexer->file_buffer[pc++])
								 {
									 case '[': ++nested_presence; break; // Adds another layer of loops.
									 case ']': --nested_presence; break; // Exits the layers of loops.
								 }
							 }
						 }
						 else
						 // If the cell value is anything but 0, then we push the program counter to keep track of it.
							 stack_push(stack, pc);

						 break;
			case R_BRAC:
						 // Once the current cell reaches a non-zero value, we jump to the pushed value aka the top of the stack.
						 if(current_memory_cell->value)
							pc = peek_stack(stack);
						 else
							 // and then decrement the value off of the stack.
							 stack_pop(stack);
						 break;
		}
	}
	putchar('\n');

	// Garbage collection:
	free_list(current_memory_cell);
	free_stack(stack);
}

// Initializes the structure
bf_lexer_t *init_lexer(FILE *fp)
{
	// Allocation & sanity checking:
	bf_lexer_t *lexer = calloc(1, sizeof(bf_lexer_t));
	LASSERT(lexer, "lexer allocation failed!\n");

	// Allocates the linked list pointer for the lexer structure:
	lexer->memory_head = init_memory();

	// Getting the size of the file:
	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	rewind(fp);

	// Setting the current contexted values into the structure:
	lexer->file_buffer_size = size;
	lexer->file_buffer = malloc(size + 1);
	lexer->file_buffer[size] = '\0';

#ifdef DEBUG
	puts("File && memory allocation debugging: ");
	printf("%sFile size         : %s%d bytes\n", BLUE, PURP, lexer->file_buffer_size);
	printf("%sMalloc'd file size: %s%ld bytes\n", BLUE, PURP, malloc_usable_size(lexer->file_buffer));
#endif

	// Load the contents of the file into "lexer->file_buffer"
	LASSERT(fread(lexer->file_buffer, 1, size, fd) == (size_t) size, "fread() error!\n");

#ifdef DEBUG
	printf("File_buffer[%d] => %s\n", size, lexer->file_buffer);
#endif

	// Then return the structure context:
	return (lexer);
}

// Garbage collection:
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

// Moves the current cell forward:
memory_t *next_cell(memory_t *cell)
{
	// Allocate memory if there is none:
	if(cell->fd == NULL)
		cell->fd = init_memory();

	// Keep the position of the current cell in the list:
	cell->fd->bk = cell;
	
	// Step forward:
	cell = cell->fd;

	
	return (cell);
}

// Moves the current cell backwards:
memory_t *prev_cell(memory_t *cell)
{
	if(cell->bk == NULL)
		cell->bk = init_memory();

	cell->bk->fd = cell;
	cell = cell->bk;

	return (cell);
}

// Garbage collection:
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

// Return the value at the top of the stack:
int peek_stack(stack_t *stack)
{
	return (stack->stack[stack->sp]);
}

// Determines if the stack is empty ot not.
int is_empty_stack(stack_t *stack)
{
	return (stack->sp < 0);
}

// Pushes a value on the top of the stack:
void stack_push(stack_t *stack, int value)
{
	stack->capacity++;

	// Extends the stack length if need be. (Dynamic sizing)
	if(stack->length == stack->capacity)
	{
		int updated_length = stack->length * 2;
		stack->stack = realloc(stack->stack, updated_length);
	}

	stack->sp++;
	stack->stack[stack->sp] = value;
}

// Removes what's at the top of the stack:
void stack_pop(stack_t *stack)
{
	LASSERT(!is_empty_stack(stack), "Stack underflow! Stack is empty\n");
	stack->sp--;
}

// Garbage collection:
void free_stack(stack_t *stack)
{
	free(stack->stack);
	free(stack);
}
