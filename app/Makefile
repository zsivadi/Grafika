CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -O2

SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)

EXEC = camping_project
LDFLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lopengl32 -lglu32 -lm

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)