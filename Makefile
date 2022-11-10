CFLAGS = -lmpdclient
CC = g++
TARGET = mpdlyrics

default: all

all: $(TARGET)

$(TARGET): mpdlyrics.cpp
	${CC} ${CFLAGS} -o $(TARGET) $<

clean:
	rm -f mpdlyrics
