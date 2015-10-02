CC = gcc
CFLAGS = -O3 -g2 -Wall -Wextra -pedantic -std=c11
LDFLAGS = -lm
RM = rm -f

CFILES = $(wildcard *.c)
HFILES = $(wildcard *.h)

.PHONY: all clean

all: pssai

pssai: $(CFILES) $(HFILES)
	$(CC) $(CFLAGS) $(CFILES) $(LDFLAGS) -o $@

clean:
	$(RM) pssai
