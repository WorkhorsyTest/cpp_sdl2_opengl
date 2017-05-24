

all: clean
	g++ main.cpp -g -std=c++14 -Werror -Wall -O2 -o main -lGL -lGLEW -lSDL2 -lSDL2_image

clean:
	rm -f main
