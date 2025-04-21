compile: build run

build:
	g++ ./src/*.cpp -o ./Epic_Knight.exe -std=c++17 -I/ucrt64/include/SDL2 -Dmain=SDL_main \
	-L/ucrt64/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

run:
	./Epic_Knight.exe
