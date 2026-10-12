CC = cc

CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -Iinclude
LDFLAGS =

SRC = src/main.c \
      src/server.c \
      src/http.c \
      src/connection.c

OBJ = $(SRC:.c=.o)

TARGET = server

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all run clean