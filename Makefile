CC = gcc
CFLAG = -Wall -Wextra -g
EXE = sudoku
OBJ = src/main.o src/cJSON.o src/utils.o

build: $(EXE)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAG)

$(EXE): $(OBJ)
	$(CC) $^ $(CFLAG) $(LDFLAGS) -utils.h -o $(EXE)

run:
	./$(EXE)

.PHONY: clean 

clean:
	rm -f src/*.o $(EXE)