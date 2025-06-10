main.cpp:
	make compile
	make run

compile:
	g++ src/main/main.cpp -lSDL3 -o target/main.out -I /usr/include/SDL3 

run:
	target/main.out

clean:
	rm -f target/*.out