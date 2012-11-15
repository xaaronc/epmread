CFLAGS=-O2 -Wall
CC=arm-linux-androideabi-gcc
BIN=epmread

default: $(BIN)

install: $(BIN)
	adb remount
	adb push $(BIN) /system/bin/

clean:
	rm -f $(BIN)

