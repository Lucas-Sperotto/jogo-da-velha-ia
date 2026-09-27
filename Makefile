CC ?= gcc
CFLAGS ?= -std=c11 -O2 -Wall -Wextra -Wpedantic
CPPFLAGS ?= -Iinclude

TARGET := jogo_velha
SRC := src/main.c src/game.c \
       src/agents/random.c src/agents/heuristic.c
OBJ := $(SRC:.c=.o)

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

test: tests/test_game tests/test_agents
	./tests/test_game
	./tests/test_agents

tests/test_game: tests/test_game.c src/game.c include/game.h
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/test_game.c src/game.c -o $@

tests/test_agents: tests/test_agents.c src/game.c src/agents/random.c src/agents/heuristic.c include/game.h include/agents.h
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/test_agents.c src/game.c src/agents/random.c src/agents/heuristic.c -o $@

clean:
	rm -f $(OBJ) $(TARGET) tests/test_game tests/test_agents
