all:
	g++ -Wall server.cpp -o server -lpthread
run:
	./server
