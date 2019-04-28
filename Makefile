CC = gcc
CFLAGS = -std=c99 -O3 -Wall -Wpedantic

EXE = image_tagger
OBJ = main.o image_tagger.o sendhttp.o


$(EXE): $(OBJ)
		$(CC) $(CFLAGS) -o $(EXE) $(OBJ)
main.o: main.c
		$(CC) $(CFLAGS) -c main.c
image_tagger.o: image_tagger.c 
		$(CC) $(CFLAGS) -c image_tagger.c

sendhttp.o: sendhttp.c 
		$(CC) $(CFLAGS) -c sendhttp.c

clean:
	rm -f $(OBJ) $(EXE)