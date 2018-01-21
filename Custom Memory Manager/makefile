# Makefile for Project 2.
# Written by Karthik Venkataramana Pemmaraju
# Compiled and tested on 11/15/2017

flags = -std=c++11 -lpthread -Wall

all: object clean
	./object

process: process.h process.cpp
	g++ process.cpp -c -o process.o -$(flags)

memory: MemoryManager.h MemoryManager.cpp
	g++ MemoryManager.cpp -c -o memory.o -$(flags)

object: main.cpp process memory
	g++ main.cpp process.o memory.o -$(flags) -o object


clean: 
	rm *.o