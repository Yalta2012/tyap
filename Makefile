CC = g++
file.cpp = l0.cpp
input = test.txt
output = aoutput.txt
FlAGS = -g

all:
	$(CC) $(file.cpp) $(FLAGS) $(FLAGS)
	./a.out $(input) $(output)