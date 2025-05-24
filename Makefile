CC = gcc
CFLAGS = -Wall -g
BINARY = icsh
OBJS = icsh.o icsh_builtins.o

all: $(BINARY)

$(BINARY): $(OBJS)
	$(CC) -o $(BINARY) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -f $(BINARY) *.o

