CFLAGS = -lmpdclient -lstdc++
CC = cc

default: all

all: mpdlyrics

mpdlyrics: mpdlyrics.cpp
	${CC} ${CFLAGS} -o mpdlyrics $<

clean:
	rm -f mpdlyrics
