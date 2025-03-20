CC = gcc
CFLAGS = -Wall -ansi -pedantic -g

SRCS =Utils.c main.c PreAssembler.c FirstScan.c
OBJS = $(SRCS:.c=.o)
HDRS = Utils.h Constants.h PreAssembler.h FirstScan.h

all: assembler

assembler: $(OBJS)
	$(CC) $(OBJS) -o assembler $(CFLAGS)

%.o: %.c $(HDRS)
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f $(OBJS) assembler
