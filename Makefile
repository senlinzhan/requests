main: main.o
	clang++ -std=c++11 -o main main.o -lboost_system  -lpthread
main.o: main.cpp Requests.hpp Response.hpp Utils.hpp
	clang++ -std=c++11 -c main.cpp
clean:
	rm -f *.o main
