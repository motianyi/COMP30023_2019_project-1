CC = gcc
CFLAGS = -std=c99 -O3 -Wall -Wpedantic

EXE = image_tagger
OBJ = main.o image_tagger.o sendhttp.o game.o string_manipulation.o


$(EXE): $(OBJ)
		$(CC) $(CFLAGS) -o $(EXE) $(OBJ)
main.o: main.c
		$(CC) $(CFLAGS) -c main.c
image_tagger.o: image_tagger.c 
		$(CC) $(CFLAGS) -c image_tagger.c

sendhttp.o: sendhttp.c 
		$(CC) $(CFLAGS) -c sendhttp.c
game.o: game.c 
		$(CC) $(CFLAGS) -c game.c
string_manipulation.o: string_manipulation.c 
		$(CC) $(CFLAGS) -c string_manipulation.c
clean:
	rm -f $(OBJ) $(EXE)