hashtable_dir = ../hashtable/
holdall_dir = ../holdall/
CC = gcc
CFLAGS = -std=c11 \
  -Wall -Wconversion -Werror -Wextra -Wfatal-errors -Wpedantic -Wwrite-strings \
  -O2 \
  -I$(hashtable_dir) -I$(holdall_dir)
LDFLAGS =
vpath %.c $(hashtable_dir):$(holdall_dir)
vpath %.h $(hashtable_dir):$(holdall_dir)
objects = hashtable.o holdall.o main.o
executable = ws

all: $(executable)

clean:
	$(RM) $(objects) $(executable)

$(executable): $(objects)
	$(CC) $(objects) $(LDFLAGS) -o $(executable)

hashtable.o: hashtable.c hashtable.h
holdall.o: holdall.c holdall.h
main.o: main.c hashtable.h holdall.h
