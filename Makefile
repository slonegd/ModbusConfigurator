all: clean build run

clean:
	-rm -fR ./a.out

build:
	g++ main.cpp -D_XOPEN_SOURCE_EXTENDED -std=c++1y -I. -lncursesw

run:
	./a.out
