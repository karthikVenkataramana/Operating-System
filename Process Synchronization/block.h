/*
  =========================================================================================
  * AUTHORS: KARTHIK VENKATARAMANA PEMMARAJU & BHARATH RAMAGONI
  * DATE: 10/08/2017 - 10/13/2017
  * DESCRIPTION: Data structure representation of a block in the shared memory.
  ===========================================================================================
*/

#include <iostream>
using namespace std;

/**
  * @Type: class
  * @Data members:
  *      Data - An integer value
  *      next - Pointer to the next Block
*/

class Block{
  public:
      long data;
      Block *next;
      Block(long y){
        data = y;
        next = NULL;
      }
};
