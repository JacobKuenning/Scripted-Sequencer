all: prog

prog: main.o script.o sequencer.o
	g++ -o prog main.o script.o sequencer.o -lrtmidi
	rm *.o

main.o: main.cpp
	g++ -c main.cpp

script.o: script.cpp
	g++ -c script.cpp

sequencer.o : sequencer.cpp
	g++ -c sequencer.cpp

run:
	./prog text.txt