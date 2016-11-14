all: mp3

mp3: main.o
	g++ main.o -o mp3

main.o: main.cpp
	g++ -c main.cpp

clean:
	rm -rf *o mp3
