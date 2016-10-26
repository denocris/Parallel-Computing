CC=gcc
CFLAGS=-O3
TARGET=exec

SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

all: $(TARGET)

%.o: %.c
	$(CC) -c $< $(CFLAGS)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS)

clean:
	@rm -f *~ *.o $(TARGET) solution.dat &>/dev/null

plot:
	@gnuplot -p plot.plt

.PHONY: clean plot

