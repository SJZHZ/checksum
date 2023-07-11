CC = g++ -std=c++11
OBJ = *.o
EXE = checksum

all:${EXE}

checksum: checksum.cpp
	$(CC) -o $@ $^ $(FLAGS) -Ofast -fopenmp -Ofast -march=native

clean:
	rm -f $(OBJ) $(EXE)