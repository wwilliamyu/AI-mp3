all: mp3

mp3: main.o #image_set.o
	g++ main.o -o mp3

main.o: image.h main.cpp
	g++ -c main.cpp

#imageSet.o: image_set.h image_set.cpp
#	g++ -c image_set.cpp

clean:
	rm -rf *o mp3
