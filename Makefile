CC ?= gcc
CFLAGS ?= -std=c11 -O2 -Wall -Wextra -Wpedantic
CPPFLAGS ?= -Iinclude

TARGET := jogo_velha
AI_SRC := src/agents/random.c src/agents/heuristic.c \
          src/agents/minimax.c src/agents/alphabeta.c \
          src/agents/features.c src/agents/samuel.c src/agents/genetic.c
SRC := src/main.c src/game.c $(AI_SRC)
OBJ := $(SRC:.c=.o)

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

test: tests/test_game tests/test_agents tests/test_minimax tests/test_alphabeta tests/test_samuel tests/test_genetic
	./tests/test_game
	./tests/test_agents
	./tests/test_minimax
	./tests/test_alphabeta
	./tests/test_samuel

tests/test_game: tests/test_game.c src/game.c include/game.h
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/test_game.c src/game.c -o $@

tests/test_agents: tests/test_agents.c src/game.c src/agents/random.c src/agents/heuristic.c include/game.h include/agents.h
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/test_agents.c src/game.c src/agents/random.c src/agents/heuristic.c -o $@

tests/test_minimax: tests/test_minimax.c src/game.c src/agents/minimax.c include/game.h include/agents.h
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/test_minimax.c src/game.c src/agents/minimax.c -o $@

tests/test_alphabeta: tests/test_alphabeta.c src/game.c src/agents/minimax.c src/agents/alphabeta.c include/game.h include/agents.h
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/test_alphabeta.c src/game.c src/agents/minimax.c src/agents/alphabeta.c -o $@

tests/test_samuel: tests/test_samuel.c src/game.c src/agents/features.c src/agents/samuel.c include/game.h include/learning.h include/samuel.h
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/test_samuel.c src/game.c src/agents/features.c src/agents/samuel.c -o $@

clean:
	rm -f $(OBJ) $(TARGET) tests/test_game tests/test_agents tests/test_minimax tests/test_alphabeta tests/test_samuel
