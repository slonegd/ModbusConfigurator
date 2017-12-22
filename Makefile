all: clean build run

clean:
	-rm -fR ./a.out

build: clean
	g++ main.cpp -Wall -g -std=c++1y -I. -I./lib -I./lib/ncursesWidgets -lncursesw

run:
	./a.out
