CC=gcc
CFLAGS=-O3
TARGET=exec

# inputs for the executables
dim = 10
it  = 100
r = 3
c = 3

SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

all: $(TARGET)

run: all
	./$(TARGET) $(dim) $(it) $(r) $(c)

%.o: %.c
	$(CC) -c $< $(CFLAGS)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS)

clean:
	@rm -f *~ *.o $(TARGET) solution.dat &>/dev/null

plot:
	@gnuplot -p plot.plt

.PHONY: clean plot

