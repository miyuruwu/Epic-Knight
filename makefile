compile: build run

build:
	g++ ./src/main.cpp -o ./out/game.exe -lSDL3 -lSDL3_image
run:
	./out/game.exe