all: midi_test

a.out: main.cpp
	g++ main.cpp -o midi_test -lrtmidi -lpthread

run: midi_test
	./midi_test