main: main.o Label.o Cpu.o Memory.o RunCommand.o
	g++ *.o -o main -lSDL2 -lSDL2_ttf -lSDL2_image -pthread

clean:
	rm *.o main

main.o: main.cpp
	g++ -c main.cpp -o main.o

Label.o: Label.hpp Label.cpp
	g++ -c Label.cpp -o Label.o

Cpu.o: Cpu.hpp Cpu.cpp
	g++ -c Cpu.cpp -o Cpu.o

Memory.o: Memory.hpp Memory.cpp
	g++ -c Memory.cpp -o Memory.o

RunCommand.o: RunCommand.hpp RunCommand.cpp
	g++ -c RunCommand.cpp -o RunCommand.o