CC = g++-13
file.cpp = lab05.cpp
input = lab05in.txt
output = output.txt
FLAGS = -g -std=c++20

all:
	$(CC) $(file.cpp) $(FLAGS)
	./a.out $(input) $(output)

val: 
	$(CC) $(file.cpp) $(FLAGS)
	valgrind --leak-check=full ./a.out $(input) $(output)
