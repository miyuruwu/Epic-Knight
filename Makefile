compile: build run

build:
	g++ ./src/*.cpp -o ./Epic_Knight.exe -lSDL2 -lSDL2_image -lSDL2_ttf -mwindows
run:
	./Epic_Knight.exe