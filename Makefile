CC = cc

CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -Iinclude

SRC = src/main.c \
      src/server.c \
      src/http.c \
      src/connection.c

OBJ = $(SRC:.c=.o)

TARGET = server

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean