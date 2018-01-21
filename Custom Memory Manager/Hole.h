/**
 *  Authors: Karthik Venkataramana Pemmaraju & Bharath Ramagoni
 *  Date: 11/18/2017
 *  Description:  Defines a Hole data structure.
**/

#include <iostream> 
#include <stdint.h>

class  Hole
{
    public: 
        uintptr_t starting_address; // Start of Hole.
        uintptr_t ending_address;  // End of Hole.
        long size;
    public:
        Hole(uintptr_t starting_address , uintptr_t ending_address, long size) // Constructor to initialize initial values.
        {
            this -> starting_address = starting_address;
            this -> ending_address = ending_address;
            this -> size = size;
        }
};