#include "process.h"

int Process::getId()
{
    return this->id;
}
int Process::getCycles()
{
    return this -> cycles;
}
void* Process::getData()
{
    return this -> data;
}

long Process::getMemorySize()
{
    return this -> memorySize;
}

void Process::setData(void*& data)
{
    this -> data = data;
}

int Process::getCompletedCycles()
{
    return this -> current_cycles;
}

void Process::setCompletedCycles(int completedCycles)
{
    this -> current_cycles += completedCycles; // Adds to previous completed cycles.
}