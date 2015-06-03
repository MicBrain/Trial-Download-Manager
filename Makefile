all: bin/test bin/main

bin/test: test.c
	gcc test.c -o bin/test -lcurl

bin/main: main.cpp Regular_Request.cpp Pipelined_Request.cpp
	g++ -std=c++11 main.cpp Regular_Request.cpp Pipelined_Request.cpp -o bin/main -lcurl