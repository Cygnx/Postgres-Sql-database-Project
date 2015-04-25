all: parse answer

parse: parse.cpp
	g++ -lpqxx -O3 parse.cpp -o parse.out
answer: answer.cpp
	g++ -lpqxx -O3 answer.cpp -o answer.out
