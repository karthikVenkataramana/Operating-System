/**
 *  Authors: Karthik Venkataramana Pemmaraju & Bharath Ramagoni
 *  Date: 11/26/2017
 *  Description:  Implementation of Memory Manager.
**/
#include "MemoryManager.h"
#include <sys/mman.h>
#include <iostream>
#include <stdlib.h>
#include <errno.h> 
#include <string.h>
#include <thread>
#include <mutex>

MemoryManager::MemoryManager(bool flag) // Flag = true for Static partitioning, False for Dynamic partitioning.
{
    data = malloc(CHUNK); // Initially, get 100 MB
    initial_address = reinterpret_cast<uintptr_t>(data); // Store initial address.
    current_address = initial_address;
    max_address = current_address + (CHUNK); // Initial + 100 MB = Final Address.
    holes.clear();
    if(flag){
        for(int i = 1; i <= 20; i++){ 
           fixedPartitions.push_back(current_address + (i * FIXEDPARTITION)); // 0 MB, 5 MB, 10 MB etc.. (20 *5 = 100 MB)
        }
    }  

}

/*
 * Description: Dynamic Allocation Manager which allocates memory of given size 
 * @return : A void pointer which can type casted to desired type. 
*/
void* MemoryManager::my_dynamic_malloc(long size)
{
    void* memory = NULL; // Memory to be allocated.
    std::mutex mutex; // Ensure only one process is in CS.
    mutex.lock();
    if(size > CHUNK ){
        std:: cout << " DYNAMIC MEMORY MANAGER CANNOT ALLOCATE " << size / (1024.0 * 1024) <<" MB "; // More than 100 MB? (Seriously?)
        exit(0);
    }
    if( (memory = allocateHoleToProcess(size)) != NULL) return memory; // First - Fit. Find any hole which can accomodate our process.
    if(max_address - current_address <= 0.25 * CHUNK){ // Perform compaction if 75% full.
        compactHoles();
    }
    if(current_address + size <= max_address){ // While we do not cross our limit of 100 MB.
        memory =  reinterpret_cast<void *>(current_address);
        current_address += size; // Increment current pointer.
    }
    mutex.unlock();
    return memory;
}

void MemoryManager::my_dynamic_free(void*& ptr, long size)
{  
    /*
    * The leaving process, creates an hole (or External Fragmentation).
    */
   createHole(reinterpret_cast<uintptr_t>(ptr), reinterpret_cast<uintptr_t>(ptr) + size, size);
}

/*
 * Dynamic partitioning implements compaction. This runs with time complexity of O(H^2) where H is the number of holes. 
 * This is not ideally the best algorithm. Nevertheless is simple to implement and view. (TO -DO: Mark and Sweep Algorithm for Compaction?) 
*/
void MemoryManager::compactHoles()
{  
    for(unsigned t = 0; t < holes.size(); t++){
        for(unsigned t1 = 0 ; t1 < holes.size() && t != t1 ;   t1++){
            if( holes[t] -> ending_address == holes[t1] -> starting_address){ // |t | t1| (Hole t followed by Hole t1)
                holes[t] -> ending_address = holes[t1] -> ending_address;
                holes[t] -> size += holes[t1]->size;    
                holes.erase(holes.begin() + t1); // Can safely erase t1 now. 
            }
            else if(holes[t1] -> ending_address == holes[t] -> starting_address){ // | t 1| t | 
                holes[t1] -> starting_address = holes[t1] -> starting_address;
                holes[t1] -> size += holes[t] -> size;
                holes.erase(holes.begin() + t1); // Can safely erase t1 now.
            }

            break;
        }
    }  
}
/*
 * Creates a New Hole in the chunk parition. 
*/
void MemoryManager::createHole(uintptr_t starting_address, uintptr_t ending_address,long size)
{ 
    Hole *hole = new Hole(starting_address, ending_address, size);
    holes.push_back(hole);   
}

/*
 * Finds the first hole which can fit the process.
*/

void* MemoryManager::allocateHoleToProcess(long size)
{
    void* memory = NULL; 
    for(unsigned t = 0; t < holes.size(); ++t){ 
        if(holes[t] -> ending_address <= max_address  && holes[t] -> size >= size){
            memory = reinterpret_cast<void *>(holes[t] -> starting_address); // Assigning memory to the process.
            if(holes[t] -> size > size) // Creates a smaller hole of remaining size (Internal fragmentation).
            {
                holes[t] -> starting_address += size;
                holes[t] -> size -= size;
            }
            else if (holes[t] -> size == size ) holes.erase(holes.begin() + t); // Remove hole, now that process is assigned to it.
            return memory;
        }
    } 
    return memory;
}
/*
 * Memory allocation using static partitioning. 
*/
void* MemoryManager::my_static_malloc(long size){ 
    void* memory; // Memory to be allocated. (First - Fit)
    std::mutex mutex;
    mutex.lock(); 
    if(size > FIXEDPARTITION){
        std:: cout << " CANNOT ALLOCATE PROCESS OF SIZE " << size / (1024* 1024.0) << " MB AS FIXED PARTITION IS OF SIZE " << FIXEDPARTITION / (1024 * 1024) << " MB" << std::endl; 
        exit(0);
    }
    if((memory = allocateHoleToProcess(size)) != NULL)    return memory; // Find Hole which fits (First - fit).
    if(current_address <= max_address){  
        memory =  reinterpret_cast<void *>(current_address); // Assign memory.
        createHole(current_address + size, current_address + FIXEDPARTITION, FIXEDPARTITION - size); // Start address is current_address + size.
        current_address += FIXEDPARTITION; // Increment by 5 MB.
        return memory;
    }   
    mutex.unlock();
    return nullptr;
}

void MemoryManager::printHoleSizes(){ 
     for(unsigned t = 0; t < holes.size(); ++t){
        std::cout << " HOLE #: " << t << " STARTING ADDRESS  " << holes[t] -> starting_address << " ENDING ADDR "  << holes[t] -> ending_address<<" SIZE: " << holes[t] -> size / (1024.0 * 1024) << " MB "<< std::endl;
     }  
}

bool MemoryManager::isStaticPartitionAddress(uintptr_t address){
    for(unsigned t = 0 ; t < fixedPartitions.size(); ++t){
        if(address ==  fixedPartitions[t] ) return true;   
    } 
    return false;
}

void MemoryManager::my_static_free(void* ptr, long size){ 
    /*
        This leaving process, leaves holes behind. Check for neighbouring holes, if any then coalesce. 
    */ 
    bool flag = false;  
    uintptr_t ptrAddress = reinterpret_cast<uintptr_t> (ptr);  
    for(unsigned  t = 0; t <  holes.size(); t++){ 
        //  |H | P | H| ( Hole followed by process followed by Hole).
        if((holes[t] -> ending_address == ptrAddress) && (!isStaticPartitionAddress(ptrAddress))){
            for(unsigned t1 = 0 ; t1 < holes.size() && t1 != t; t1++){
                if(holes[t1] -> starting_address == ptrAddress ){
                    // Combine 3 of them into one large Hole. 
                    holes[t] -> ending_address = holes[t1] -> ending_address;
                    holes[t] -> size += (size + (holes[t1] -> size));
                    // erase hole t1 and exit out of my_free() method.
                    holes.erase(holes.begin() + t1);
                    flag = true;
                    return;
                }
            }
        }
        if((holes[t] -> ending_address == ptrAddress) && (!isStaticPartitionAddress(ptrAddress)) ){ // Found a neighbouring hole which belongs to our partition. We can coalesce now. 
            // A Hole followed by Process | H | P |   
            holes[t] -> ending_address = ptrAddress + size;
            holes[t] -> size += size;
            flag = true; 
        }
        else if((holes[t] -> starting_address == ptrAddress + size) && (!isStaticPartitionAddress(ptrAddress + size))){ // A Process followed by Hole | P | H | 
            holes[t] -> starting_address = ptrAddress;
            holes[t] -> size += size;
            flag = true; 
        }
    } 
    if(!flag) // No Neighbouring holes, Now, we have to create one.
     {    
       createHole(reinterpret_cast<uintptr_t>(ptr), reinterpret_cast<uintptr_t>(ptr) + size, size);
     }
    
}
     
void MemoryManager::freeMemory()
{ 
    free(data); // free our requested 100 MB initial request.
}

 