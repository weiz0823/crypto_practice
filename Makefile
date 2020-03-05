# Just an empty makefile now.
# Remember to use TAB to indent.
CXX=g++
CXXFLAGS=-std=c++2a -O2

all:
	echo "Nothing to compile now"

compile/bigint.o: src/bigint.cpp src/bigint.h
	$(CXX) $(CXXFLAGS) -c src/bigint.cpp -o compile/bigint.o

.PHONY: all clean
clean:
	rm compile/*
