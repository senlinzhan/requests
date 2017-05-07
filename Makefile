main: main.o
	clang++ -g -std=c++11 -Wall -o main main.o -lboost_system  -lpthread
main.o: main.cpp Request.hpp Response.hpp Utils.hpp AsyncRequest.hpp
	clang++ -g -std=c++11 -Wall -c main.cpp
clean:
	rm -f *.o main
