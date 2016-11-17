

all: clean
	g++ main.cpp -std=c++11 -Werror -Wall -O2 -o main -lGL -lGLEW -lSDL2 -lSOIL

clean:
	rm -f main
