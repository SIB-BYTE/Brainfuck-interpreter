/* TODO:
1.) Fix execute(); function. It's not executing shit.
2.) Make things look pretty, maybe make a debug API.
3.) Profit? ???
*/

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

bf_lexer_t *current_token = {0};

int main(int argc, char **argv)
{
	LASSERT(argc == 2, "Usage: %s <brainfuck file>\n", argv[0]);

	FILE *fd = fopen(argv[1], "r");
	LASSERT(fd != NULL, "Failure to open file\n");

	init_lexer(fd);

	bf_compile();

	#ifdef DEBUG
	display_token_stream();
	#endif

	bf_execute();

	destroy_lexer();
}

void bf_compile(void)
{
	for(int pc = 0; pc < current_token->file_buffer_size; ++pc)
	{
		switch(current_token->file_buffer[pc])
		{
			case INC:    lexer_push_token(INC);     break;
			case DEC:    lexer_push_token(DEC);     break;
			case INC_DP: lexer_push_token(INC_DP);  break;
			case DEC_DP: lexer_push_token(DEC_DP);  break;
			case PRINT:  lexer_push_token(PRINT);   break;
			case READ:   lexer_push_token(READ);    break;
			case L_BRAC: lexer_push_token(L_BRAC);  break;
			case R_BRAC: lexer_push_token(R_BRAC);  break;
			default: pc++; break;
		}
	}
	#ifdef DEBUG
	printf("%sToken stream malloc'd size: %s%ld bytes\n\n", BLUE, PURP, malloc_usable_size(current_token->token_stream));
	#endif
}

void bf_execute(void)
{
	int loop_presence = 0;
	char *current_instruction = current_token->token_stream;

	for(int pc = 0; current_token->token_stream[pc] != '\0'; ++pc)
	{
		char token_ptr = current_token->token_stream[pc];
		switch(token_ptr)
		{
			case INC:    ++(*current_instruction); break;
			case DEC:    --(*current_instruction); break;
			case INC_DP: ++current_instruction;    break;
			case DEC_DP: --current_instruction;    break;
			case PRINT:  putchar(*current_instruction); fflush(stdout); break;
			case READ:   *current_instruction = getchar(); break;
			case L_BRAC:
						 if(!token_ptr)
						 {
							 loop_presence = 1;
							 token_ptr = current_token->token_stream[++pc];

							 do
							 {
								switch(token_ptr)
								{
									case '[': ++loop_presence; break;
									case ']': --loop_presence; break;
								}
							 } while(loop_presence);
						 }
						 break;

			case R_BRAC:
						 loop_presence = 1;
						 token_ptr = current_token->token_stream[--pc];

						 if(token_ptr)
						 {
							 do
							 {
								 switch(token_ptr)
								 {
									 case '[': --loop_presence; break;
									 case ']': ++loop_presence; break;
								 }
							 } while(loop_presence);
						 }
						 break;
		}
	}
}

void lexer_push_token(char token)
{
	LASSERT(current_token != NULL, "lexer_push_token(): Lexer is NULL\n");

	if(current_token->token_stream_length == current_token->token_stream_capacity)
	{
		current_token->token_stream_capacity = current_token->token_stream_capacity * 2 + 1;
		char *new_token_stream = realloc(current_token->token_stream, current_token->token_stream_capacity);

		LASSERT(new_token_stream != NULL, "lexer_push_token(): Memory allocation failure!\n");

		current_token->token_stream = new_token_stream;
	}

	current_token->token_stream[current_token->token_stream_length] = token;
	current_token->token_stream_length++;
}

#ifdef DEBUG
void display_token_stream(void)
{
	printf("%sToken stream debug data: \n", WHI);
	for(size_t i = 0; i < current_token->token_stream_length; ++i)
		printf("%sToken stream[%lu] => {%c}\n", RED, i, current_token->token_stream[i]);
}
#endif

void init_lexer(FILE *fd)
{
	current_token = calloc(1, sizeof(bf_lexer_t));
	LASSERT(current_token != NULL, "init_lexer(): current_token allocation failed!\n");

	int size = set_file_size(fd);

	current_token->file_buffer = malloc(size + 1);
	current_token->file_buffer[size] = '\0';


	#ifdef DEBUG
		puts("File && memory allocation debugging: ");
		printf("%sFile size         : %s%d bytes\n", BLUE, PURP, current_token->file_buffer_size);
		printf("%sMalloc'd file size: %s%ld bytes\n", BLUE, PURP, malloc_usable_size(current_token->file_buffer));
	#endif

	LASSERT(current_token != NULL && current_token->file_buffer != NULL, "init_lexer(): memory allocation error!\n");
	LASSERT(fread(current_token->file_buffer, 1, size, fd) == (size_t) size, "init_lexer(): fread() error!\n");

}

void destroy_lexer(void)
{
  free(current_token->token_stream);
  free(current_token->file_buffer);
  free(current_token);
}

int set_file_size(FILE *fd)
{
	struct stat file = {0};

	int file_fd = fileno(fd);
	LASSERT(file_fd != -1, "fileno() error!\n");

	LASSERT(fstat(file_fd, &file) != -1, "get_file_size(): Failure to get size!\n");
	current_token->file_buffer_size = file.st_size;

	return (file.st_size);
}

