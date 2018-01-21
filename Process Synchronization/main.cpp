/*
  =========================================================================================
  * AUTHORS: KARTHIK VENKATARAMANA PEMMARAJU & BHARATH RAMAGONI
  * DATE: 10/08/2017 - 10/13/2017
  * DESCRIPTION: A semaphore based solution for a synchronizing
                 three processes to operate on three common shared resources.
  * COMPILATION AND EXECUTION: make
  * CITATION: https://stackoverflow.com/questions/1641182/how-can-i-catch-a-ctrl-c-event-c
  ===========================================================================================
*/

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <signal.h>
#include "list.h"
using namespace std;

#define N 100

void initializeSemaphores();
void printSizes();
void cleanup();
void eventHandler(int);
char input;

Block *memoryChunks;
list *freeList, *list1, *list2; // 3 shared resources.
int process1Id, process2Id, process3Id; // list of process ID's
sem_t *p1, *fl, *l1, *l2, *flMutex, *l1Mutex, *l2Mutex; // list of semaphores
int fd, dupFd; // File descriptors.

/**
  @params:
      1) name - A constant character pointer to represent a string.
      2) size - size of the shared memory to be created.
  @return: Returns the pointer to the data type for which shm is created.
*/
template <typename T>
  T* createSharedMemoryForVariables(const char * name, int size) {
    T *nameObject;
    int fd = shm_open(name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IXUSR); // Shm_open establishes connection between fd(file descriptor) and memory object (sharedRegion)
    ftruncate(fd, size * sizeof(T)); // truncates to the size of semaphore.
    nameObject = (T *) mmap(NULL, sizeof(T), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); // maps files specified by file descriptor onto memory (NULL specifies that kernel chooses an address to us).
    return nameObject;
  }

/**
  @desc: Emulates the function of first process as given in the project document.
*/
void processOne() {
  Block *b;
  while (true) {
    sem_wait(p1); // Semaphore P1 ensures that P1 can execute atmost N -1 times.
    sem_wait(fl); // Check if free list can be unlinked.
    sem_wait(flMutex);
    b = freeList -> unlink();
    sem_post(flMutex);
    b -> data += 1; // Produce information onto 'B'
    sem_wait(l1Mutex);
    list1 -> link(b);
    printSizes();
    sem_post(l1Mutex);
    sem_post(l1); // Raise hand that l1 is ready to unlink.
  }
}

/**
  @desc: Emulates the function of second process as given in the project document.
*/
void processTwo() {
  Block *x, *y;
  while (true) {
    sem_wait(l1); // Check if l1 can be unlinked.
    sem_wait(l1Mutex);
    x = list1 -> unlink();
    sem_post(l1Mutex);
    sem_wait(fl); // Check if fl can be unlinked.
    sem_wait(flMutex);
    y = freeList -> unlink();
    sem_post(flMutex);
    y -> data = x -> data % 9999; // Using x to produce info in y
    sem_post(p1); // Inform P1 can run after atleast P2 has linked a block of freelist.
    sem_wait(flMutex);
    freeList -> link(x);
    sem_post(flMutex);
    sem_post(fl); // Raise hand informing freeList is ready to unlink.
    sem_wait(l2Mutex);
    list2 -> link(y);
    printSizes();
    sem_post(l2Mutex);
    sem_post(l2); // Raise hand informing l2 is ready to unlink.
  }
}

/**
  @desc: Emulates the function of third process as given in the project document.
*/
void processThree() {
  Block *c;
  while (true) {
    sem_wait(l2); // Check if l2 can be unlinked.
    sem_wait(l2Mutex);
    c = list2 -> unlink();
    sem_post(l2Mutex);
    cout << "CONSUMED: " << c -> data << endl; // Consuming data in c.
    sem_wait(flMutex);
    freeList -> link(c);
    printSizes();
    sem_post(flMutex);
    sem_post(fl); // Raise hand that freeList is ready to unlink.
  }
}

/**
  @desc: Each process is associated with each function here.
*/
void callProcesses() {
  int pid = getpid();
  if (pid == process1Id)
    processOne();
  else if (pid == process2Id)
    processTwo();
  else
    processThree();
}

/**
  @param: number - number of processes to be created.
  @desc: Creates the given number of processes by forking.
*/
void createProcesses(int number) {
    int i;
    pid_t pid;
    process1Id = getpid(); // Main process.
    for (i = 1; i < number; ++i) {
      pid = fork();
      if (pid == 0) { // Get the child processes.
        if (i == 1)
          process2Id = getpid();
        if (i == 2)
          process3Id = getpid();
        break;
      }
      if (pid < 0) break;
    }
  }
/**
    @desc: Handles the user interrupt event handler.
*/
void OnQuitPressed() {
  struct sigaction sigIntHandler;
  // Ctrl - C event handler code (Taken from Stack Overflow).
  sigIntHandler.sa_handler = eventHandler;
  sigemptyset( & sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;
  sigaction(SIGINT, & sigIntHandler, NULL);
}

int main(int argc, char const * argv[]) {
  int i;
  time_t t = time(NULL);
  struct tm tm = * localtime( & t);
  mode_t mode = S_IRUSR | S_IWUSR | S_IXUSR; // Read, write and execute permissions to owner.
  memoryChunks = createSharedMemoryForVariables <Block> ("/memoryChunks", N); // N chunks of type Block.
  freeList = createSharedMemoryForVariables <list> ("/freeList", 1); // One class pointer to free list.
  list1 = createSharedMemoryForVariables <list> ("/list1", 1);
  list2 = createSharedMemoryForVariables <list> ("/list2", 1);
  // Now, let's create shared memory for our semaphores.
  p1 = createSharedMemoryForVariables <sem_t> ("/p1", 1);
  flMutex = createSharedMemoryForVariables <sem_t> ("/flMutex", 1);
  l1Mutex = createSharedMemoryForVariables <sem_t> ("/l1Mutex", 1);
  l2Mutex = createSharedMemoryForVariables <sem_t> ("/l2Mutex", 1);
  fl = createSharedMemoryForVariables <sem_t> ("/fl", 1);
  l1 = createSharedMemoryForVariables <sem_t> ("/l1", 1);
  l2 = createSharedMemoryForVariables <sem_t> ("/l2", 1);
  freeList -> assign(memoryChunks, N); // Initially, assign N memoryChunks to freeList.
  initializeSemaphores();
  OnQuitPressed();
  cout << "Do you want to redirect output onto terminal or onto a file?\nF - File\nT - Terminal\n";
  cin >> input;
  if (input == 'F') {
    fd = open("/tmp/logFile.txt", O_CREAT | O_RDWR | O_TRUNC, mode);
    cout << "ATTENTION: Running me for 10 seconds would give you 50 MB log file." << endl;
    cout << "Writing to the logFile.txt in /tmp directory ........." << endl;
    dupFd = dup(1);
    dup2(fd, 1);
    printf("****LAST RUN ON********* \nDATE: %d-%d-%d TIME: %d:%d \n \n", tm.tm_mon + 1, tm.tm_mday, tm.tm_year + 1900, tm.tm_hour, tm.tm_min);
  }
  createProcesses(3); // Create three processes.
  callProcesses(); // Call our near & dear functions.

  return 0;
}

/**
  @desc: Prints the size of each of the three shared memory resources.
*/
void printSizes() {
  int freeListSize = freeList -> size();
  int list1Size = list1 -> size();
  int list2Size = list2 -> size();
  int total = freeListSize + list1Size + list2Size;
  if (getpid() == process1Id)
    cout << "-----------------NOW SERVING: P1 ---------------" << endl;
  else if (getpid() == process2Id)
    cout << "-----------------NOW SERVING: P2 ---------------" << endl;
  else
    cout << "-----------------NOW SERVING: P3 ---------------" << endl;
  cout << "FL: " << freeListSize << endl;
  // freeList -> display(); // Used for debugging, can be uncommented and tested individually.
  cout << "L1: " << list1Size << endl;
  // list1 -> display();
  cout << "L2: " << list2Size << endl;
  // list2 -> display();
  cout << "TOTAL: " << total << endl;
}

/**
  @desc: Initializes the semaphores to their initial values.
*/
void initializeSemaphores() {
    int pShared = 1; // shared between processes.
    sem_init(p1, pShared, N - 1); // To ensure process 1 executes N-1 times atmost
    sem_init(fl, pShared, N); // N blocks for free list
    sem_init(l1, pShared, 0); // N blocks for list 1 but initially empty
    sem_init(l2, pShared, 0); // Same as list 1
    sem_init(flMutex, pShared, 1); // To enforce M.E on Free list.
    sem_init(l2Mutex, pShared, 1); // To enforce M.E on list 2.
    sem_init(l1Mutex, pShared, 1); // To enforce M.E on list 1.
}
/**
    @desc: Clean up the shared memory and destroy semaphores to eliminate abnormal behaviours.
*/
void cleanup() {
  // Unlink allocated memory.
  shm_unlink("memoryChunks");
  shm_unlink("list1");
  shm_unlink("list2");
  shm_unlink("freeList");
  // Now unlink semaphores.
  shm_unlink("fl");
  shm_unlink("l1");
  shm_unlink("l2");
  shm_unlink("flUnlink");
  shm_unlink("l1Unlink");
  shm_unlink("l2Unlink");
  shm_unlink("flMutex");
  shm_unlink("l1Mutex");
  shm_unlink("l2Mutex");
  shm_unlink("p1");
  // Don't forget to destroy the semaphores!
  sem_destroy(p1);
  sem_destroy(fl);
  sem_destroy(l1);
  sem_destroy(l2);
  sem_destroy(flMutex);
  sem_destroy(l1Mutex);
  sem_destroy(l2Mutex);
}

void eventHandler(int s) {
  cleanup(); // Empty the trash.
  wait(NULL); // Make the last process write the below cout statements.
  if (getpid() == process1Id) {
    dup2(dupFd, 1); // Writing back to terminal now.
    cout << endl << "SUCCESS!!!" << endl;
    cout << "I will clean up shared memory for you!" << endl;
    if (input == 'F')
      cout << "Saved the log file to /tmp directory" << endl;
  }
  close(fd);
  close(dupFd);
  exit(1);
}
