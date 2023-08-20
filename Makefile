CC = g++ -std=c++11
OBJ = *.o
EXE = checksum test

all:${EXE}

checksum: checksum.cpp
	$(CC) -o $@ $^ $(FLAGS) -Ofast -fopenmp -Ofast -march=native

test: test.cpp
	$(CC) -o $@ $^ $(FLAGS) -Ofast -fopenmp -Ofast -march=native

clean:
	rm -f $(OBJ) $(EXE)