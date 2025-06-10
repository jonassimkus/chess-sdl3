main.cpp:
	make compile
	make run

compile:
	g++ src/main/main.cpp -lSDL3 -o target/SDLChess.out -I /usr/include/SDL3 -I src/main/peices -I /usr/include/SDL3_image 

run:
	target/SDLChess.out

clean:
	rm -f target/*.out