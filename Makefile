CFLAGS = -lmpdclient -lstdc++
CC = clang

default: all

all: mpdlyrics

mpdlyrics: mpdlyrics.cpp
	${CC} ${CFLAGS} -o mpdlyrics $<

clean:
	rm -f mpdlyrics
