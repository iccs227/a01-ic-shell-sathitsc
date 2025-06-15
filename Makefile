CC = gcc
CFLAGS = -Wall -g
BINARY = icsh

# Add all object files here
OBJS = icsh.o \
       icsh_builtins.o \
       icsh_jobs.o \
       icsh_signal_handler.o \
       icsh_external_command.o \
       icsh_extra.o

all: $(BINARY)

$(BINARY): $(OBJS)
	$(CC) -o $(BINARY) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -f $(BINARY) *.o
