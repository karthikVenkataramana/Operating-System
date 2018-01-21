# Makefile for first project as part of course CSCE 5640.
# Written by Karthik Venkataramana Pemmaraju
# Compiled and tested on 10/13/2017

flags = lrt -std=c++11

all: clean object
	./object

object: main.cpp
	g++ main.cpp -o object -$(flags)

# Clean up the shared  memory if not already!
clean:
	rm -f object
	rm -f /tmp/logFile.txt
	rm -f /dev/shm/memoryChunks
	rm -f /dev/shm/list1
	rm -f /dev/shm/list2
	rm -f /dev/shm/freeList
	rm -f /dev/shm/fl
	rm -f /dev/shm/l1
	rm -f /dev/shm/l2
	rm -f /dev/shm/flUnlink
	rm -f /dev/shm/l1Unlink
	rm -f /dev/shm/l2Unlink
	rm -f /dev/shm/flMutex
	rm -f /dev/shm/l1Mutex
	rm -f /dev/shm/l2Mutex
	rm -f /dev/shm/p1
