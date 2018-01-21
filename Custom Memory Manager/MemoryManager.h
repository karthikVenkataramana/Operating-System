/**
 *  Authors: Karthik Venkataramana Pemmaraju & Bharath Ramagoni
 *  Date: 11/22/2017
 *  Description:  Defines the components of a Memory Manager.
**/

#include "Hole.h"
#include <cstdint>
#include <vector>

#define FIXEDPARTITION 5 * 1024 *1024 // 5 MB
#define CHUNK 100 * 1024 * 1024 // 100 MB

class MemoryManager
{
    private: 
        void* data;
        uintptr_t current_address; // Current pointer.
        uintptr_t initial_address; // Initial Address
        uintptr_t max_address; // Final Address. 
        std::vector<Hole*> holes; // List of Holes.
        std::vector<uintptr_t> fixedPartitions; // List of partitions.
    public:
        MemoryManager(bool flag); 
        void* my_dynamic_malloc(long size);
        void my_dynamic_free(void *& ptr, long size);
        void* my_static_malloc(long size);
        void my_static_free(void* ptr, long size);
        void freeMemory();
        void printStaticMemoryUsage();
        bool isStaticPartitionAddress(uintptr_t address);
        void createHole(uintptr_t, uintptr_t ,long);
        void* allocateHoleToProcess(long size);
        void printHoleSizes();
        void compactHoles();
};