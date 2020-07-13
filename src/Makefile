main: main.o Label.o
	g++ *.o -o main -lSDL2 -lSDL2_ttf -pthread
	rm *.o

clean:
	rm *.o main

main.o: main.cpp
	g++ -c main.cpp -o main.o

Label.o: Label.hpp Label.cpp
	g++ -c Label.cpp -o Label.o