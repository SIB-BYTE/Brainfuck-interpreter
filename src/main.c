#include <stdio.h>
#include "interpreter.h"

int main(int argc, char **argv)
{
	LASSERT(argc == 2, "Usage: %s <brainfuck file>\n", argv[0]);

	FILE *fd = fopen(argv[1], "r");
	LASSERT(fd, "Failure to open file\n");

	bf_lexer_t *lexer = init_lexer(fd);

	bf_compile(lexer);

#ifdef DEBUG
	display_token_stream(lexer);
#endif

	bf_execute(lexer);

	destroy_lexer(lexer);

	fclose(fd);
}

