CC = gcc
CFLAGS = -Wall -ansi -pedantic -g

SRCS = firstpass.c main.c secondpass.c preAssembler.c
OBJS = $(SRCS:.c=.o)
HDRS = firstpass.h constants.h secondpass.h preAssembler.h

all: assembler

assembler: $(OBJS)
	$(CC) $(OBJS) -o assembler $(CFLAGS)

%.o: %.c $(HDRS)
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f $(OBJS) assembler
