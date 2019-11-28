all: run

skulldeath: main.c
	gcc main.c -o skulldeath -lSDL2 -lSDL2_image

run: skulldeath
	./skulldeath

clean:
	rm -f *.o skulldeath