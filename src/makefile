CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -Og -g3 -o
DEBUG_FLAGS = -Wall -Wextra -Wpedantic -Og -g3 -fsanitize=address,undefined -D DEBUG -o
FILE           = interpreter.c
BIN            = interpreter
MAIN           = main.c
MAIN_BIN       = main
DEBUG_MAIN_BIN = main_debug

build:
	$(CC) $(FILE) $(MAIN) $(CFLAGS) $(MAIN_BIN)
debug_build:
	$(CC) $(FILE) $(MAIN) $(DEBUG_FLAGS) main_debug

