CFLAGS=-O2 -Wall
CC=arm-linux-androideabi-gcc
BIN=epmread

default: $(BIN)

clean:
	rm -f $(BIN)

