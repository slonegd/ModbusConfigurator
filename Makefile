all: clean build

clean:
	-rm -fR ./a.out

build: clean
	g++ main.cpp -Wall -g -std=c++1y -I. -I./lib -I./lib/ncursesWidgets -lncursesw

debug: clean
	g++ main.cpp -Wall -g -std=c++1y -DDEBUG -I. -I./lib -I./lib/ncursesWidgets -lncursesw

run:
	./a.out
