/**
 *  Authors: Karthik Venkataramana Pemmaraju & Bharath Ramagoni
 *  Date: 11/16/2017 - 12/1/2017
 *  Description: A Simple Process Generator generating 50 processes in the range of <10 KB, 2 MB> with number of cyles in the range of  <200 , 2500> Cycles.
 *               malloc() - A system call which allocates memory of given size.
 *               my_dynamic_malloc() - A user function which allocates memory in the user space using dynamic partitioning. (One inital malloc of 100 MB).
 *               my_static_malloc() - A user function which allocates memory using static partitioning.
 *  Execution: make
**/

#include <iostream>
#include <stdlib.h>
#include <thread>         // this_thread::sleep_for
#include <chrono>         // chrono::milliseconds
#include <vector> 
#include <numeric>        // std::accumulate

#include "process.h"
#include "MemoryManager.h"

void memoryManager(int flag); // flag = 1 for system process, 2 for dynamic partition , 3 for static partion.
void createRandomProcesses(); // Generates Random processes with the given range.
void displayProcesses(); // Displays the randomly generated processes. 

/*
 *  Vector arrays for various process states in the system.
*/ 
std::vector<Process*> processArray;   
std::vector<Process *> activeProcessArray; 
std::vector<Process *> waitingProcessArray; 

std::vector<long long> timeForSystemMemoryManager; // A vector array to store execution times for each mechanism.
std::vector<long long> timeForDynamicMemoryManager;
std::vector<long long> timeForStaticMemoryManager;

MemoryManager *managerforDynamic; // Dynamic Memory Manager.
MemoryManager *managerforStatic; // Static Memory Manager.

long long average(std::vector<long long>); 
int totalMemory = 0;
int totalCycles = 0; // Total cycles required by simulation.
int total_cycles = 0;  // Total cycles by all the processes combined.
int numProcesses = 50;

int main(){
    int iterations = 50; // Number of times simulation is performed.
    srand(time(NULL)); // seeding the random number.
    for(int i = 1; i <= iterations; i++){
        createRandomProcesses(); 
        if(i == 1) { 
            std::cout << "\nINITIAL SET OF PROCESSES:\n";
            displayProcesses();
            std::cout << "\nCOLLECTING STATISTICS: \n \n";
        }
        std::cout << "."; 
        fflush(stdout); // cout is a buffered function. Let's flush each time to display dot's individually.
        for(int j = 1; j <=3 ; j++){
            memoryManager(j);   
        }   
       
    } 
    std::cout << "\n \n \t\tSIMULATION REPORT\n\n";
    std::cout << "/*************************************************************/\n";
    std::cout << "NUMBER OF SIMULATIONS PERFORMED #" << iterations << "\n";
    std::cout << "TOTAL NUMBER OF PROCESSES PER SIMULATION: " << numProcesses << " \n";
    std::cout << "AVERAGE MEMORY SIZE PER SIMULATION: " <<   totalMemory / (iterations * 1024.0)  << " MB. \n";
    std::cout << "AVERAGE NUMBER OF CYCLES PER PROCESS:  " << total_cycles / (numProcesses *iterations) << " CYCLES. \n";
    std::cout << "AVERAGE NUMBER OF CYCLES REQUIRED PER SIMULATION TO COMPLETE:  " << totalCycles / iterations << " CYCLES. \n";
    std::cout <<  "AVERAGE SYSTEM PARTITION PERFORMANCE: " << average(timeForSystemMemoryManager) << " MICRO SEC "<< std::endl;
    std::cout <<  "AVERAGE DYNAMIC PARTITION PERFORMANCE: " << average(timeForDynamicMemoryManager) << " MICRO SEC "<< std::endl;
    std::cout <<  "AVERAGE STATIC PARTITION PERFORMANCE: " << average(timeForStaticMemoryManager) << " MICRO SEC "<< std::endl;
    std::cout << "/*************************************************************/\n";
    return 0;
}

/*
 * @Type: Function 
 * Description: Creates random processes in the following range.
 *              Memory - 10 KB to 2MB
 *              Cycles - 200 to 2500
*/
void createRandomProcesses(){
    processArray.clear(); 
    Process *process;
    int tempMemory; 
    int cycles;
    for(int i = 1; i <= numProcesses ; i++){
        tempMemory = rand() % 2039 + 10; // 10 KB to 2048 KB
        cycles = rand() % 2301 + 200; // Cycles between 200 and 2500
        process = new Process(i, tempMemory, cycles, 0); // Initial completed cycle is 0.
        totalMemory += tempMemory;  
        total_cycles += cycles;
        processArray.push_back(process);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    } 

}

/*
 * Displays the processes in the system. 
*/
void displayProcesses(){
    for(auto process: processArray){
        double memory = process -> getMemorySize(); 
        std::cout << "PROCESS: " << process -> getId() << "\t MEMORY: ";
        if(memory > 1024){
            memory = memory / 1024.0;
            std::cout << memory<<"MB\tCYCLES:";
        }
        else   std::cout << memory << "KB\t\tCYCLES:";
        std::cout << process -> getCycles() << "\n";
    }
}

/*
 *  @param - Number - Number of Cycles the process has completed.
 *  Description - Updates the number of cycles the process has completed in our simulation.
*/

void updateCyclesForActiveProcesses(int number){
    for(auto process: activeProcessArray)
        process -> setCompletedCycles(number);
}

/*
 *  @param - Current Cycles - Number of cycles the process has been in our simulation.
 *  Description - Calls the free() function for different Memory managers.
*/

void freeInactiveProcesses(int currentCycles, int flag){
    for(std::vector<Process*>::iterator it = activeProcessArray.begin(); it != activeProcessArray.end();){   
        if( (*it) -> getCycles() <= currentCycles){ 
            long size = (*it) -> getMemorySize() * 1024;
            void* data = (*it) -> getData(); 
            flag > 1 ? (flag > 2 ?  managerforStatic -> my_static_free(data, size) : managerforDynamic -> my_dynamic_free(data, size)) : free(data);    
            it = activeProcessArray.erase(it); // Remove the process which has completed it's specified number of cycles.
        }
        else 
            ++it;  
        
    }
}

void allocateMemoryForWaitingProcesses(int flag){
    for( unsigned t = 0; t < waitingProcessArray.size() ; t++ ){ // Allocate memory for processes which are waiting the waiting array.
            size_t size =  waitingProcessArray[t] -> getMemorySize() * 1024; // In bytes. 
            void* data = flag > 1 ? flag > 2 ? managerforStatic -> my_static_malloc(size) : managerforDynamic -> my_dynamic_malloc(size) : malloc(size);    
            if(data != NULL){
                waitingProcessArray.erase(waitingProcessArray.begin() + t); // Remove from the waiting list.
                waitingProcessArray[t] -> setData(data); // Set memory to our data structure.
                activeProcessArray.push_back( waitingProcessArray[t]); // Now the process is active.
            } 
    }
}

/*  IMPORTANT! (Core logic performed here.)
 *  @param - Flag: flag = 1 for System Malloc(), flag = 2 for Dynamic partition and 3 for Static partition.
 *  Description - Allocates and deallocates memory for our process data structure. Timing Analysis done inside here.
*/

void memoryManager(int flag){ 
    std::chrono::high_resolution_clock::time_point start, end; 
    int currentCycles = 0;  
    start = std::chrono::high_resolution_clock::now(); // start the clock().
    if (flag == 3) managerforStatic = new MemoryManager(true); // Create object for Memory Manager ( True - for Static, False for Dynamic).
    else if (flag == 2) managerforDynamic = new MemoryManager(false); 
    std::vector<Process*>::iterator it = processArray.begin(); 
    while(it != processArray.end()  || waitingProcessArray.size() > 0 || activeProcessArray.size() > 0){
        if(currentCycles % 50 == 0 && it != processArray.end()) { // For each process that arrives after 50 cycles. 
            size_t size = (*it) -> getMemorySize() * 1024; // In bytes.
            void* data = flag > 1 ? (flag > 2 ? managerforStatic -> my_static_malloc(size) : managerforDynamic -> my_dynamic_malloc(size)) : malloc(size);    // Allocate memory.
            if(data == NULL)
                waitingProcessArray.push_back(*it);  // Null ( Could not allocate memory now, Check later!)
            else
                (*it) -> setData(data); // If not null, then put data in our data structure.
            activeProcessArray.push_back(*it); // Process is active now.
            it++;
        }
        currentCycles++; 
        allocateMemoryForWaitingProcesses(flag);
        updateCyclesForActiveProcesses(1);
        freeInactiveProcesses(currentCycles, flag);       
        if( (currentCycles > total_cycles + 5 * numProcesses) && (flag == 2)){  // For Dynamic memory manager, if we couldn't find memory for specified amount of time, we generate system message.
            std::cout << "The following processes cannot be satisifed by Dynamic Memory Manager \n";
            for(unsigned t= 0; t < waitingProcessArray.size() ; t++)
                std::cout << "Requirement: " << waitingProcessArray[t] -> getMemorySize() / 1024.0 << " MB\n";
            break;
        }
    }   // Completes Our Simulation. 
    if(flag == 1) totalCycles += currentCycles; // Since, we need to do it only for once per flag.
    else if(flag == 2) managerforDynamic -> freeMemory(); // Free initial 100 MB request.
    else if(flag == 3) managerforStatic -> freeMemory(); 
    end = std::chrono::high_resolution_clock::now(); // End of our timer
    long timeConsumed = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count(); // Get difference and push it into an array.
    flag > 1 ? (flag > 2 ? timeForStaticMemoryManager.push_back(timeConsumed) : timeForDynamicMemoryManager.push_back(timeConsumed)) : timeForSystemMemoryManager.push_back(timeConsumed);
}

long long average(std::vector<long long int > vec){
    return (std::accumulate(vec.begin(), vec.end(), 0)) / (long long)vec.size(); // returns the average of the contents of the vector.
}

 
 