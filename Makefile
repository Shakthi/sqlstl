all:test.h test.cpp
	g++ test.cpp -o test

clean:
	rm test test.o


