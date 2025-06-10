main.cpp:
	make compile
	make run

compile:
	g++ src/main/main.cpp -lSDL3 -lSDL3_image -o target/SDLChess.out -I /usr/include/SDL3 -I src/main/peices -I /usr/include/SDL3_image -I src/main

run:
	target/SDLChess.out

clean:
	rm -f target/*.out