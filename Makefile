CC = gcc
CFLAGS = -std=c99 -O3 -Wall -Wpedantic
EXE = image_tagger
OBJ = image_tagger.o sendhttp.o


$(EXE): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJ)

sendhttp.o: sendhttp.c sendhttp.h
	$(CC) $(CFLAGS) -c sendhttp.c

image_tagger.o: image_tagger.c image_tagger.h sendhttp.h
	$(CC) $(CFLAGS) -c image_tagger.c

clean:
	rn -f $(OBJ) $(EXE)