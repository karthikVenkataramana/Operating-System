/**
 *  Authors: Karthik Venkataramana Pemmaraju & Bharath Ramagoni
 *  Date: 11/16/2017
 *  Description:  Defines a process data structure.
**/

#include <iostream> 
#include <stdint.h>
 

class Process
{
    private:
        int id;
        int memorySize;
        int cycles;
        void* data;
        int current_cycles;
    public:
        Process(int id, long memorySize, int cycles, int current_cycles)
        {
            this -> id = id;
            this -> memorySize = memorySize;
            this -> cycles = cycles;
            this -> current_cycles = current_cycles;
        }
        void* getData();
        void setData(void*& data);
        long getMemorySize();
        int getCycles();
        int getId(); 
        int getCompletedCycles();
        void setCompletedCycles(int);
};