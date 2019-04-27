CC = gcc
CFLAGS = -std=c99 -O3 -Wall -Wpedantic

EXE = image_tagger
OBJ = image_tagger.o sendhttp.o


$(EXE): $(OBJ)
		$(CC) $(CFLAGS) -o $(EXE) $(OBJ)

image_tagger.o: image_tagger.c 
		$(CC) $(CFLAGS) -c image_tagger.c

sendhttp.o: sendhttp.c 
		$(CC) $(CFLAGS) -c sendhttp.c

clean:
	rm -f $(OBJ) $(EXE)