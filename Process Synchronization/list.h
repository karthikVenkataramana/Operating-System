/*
  =========================================================================================
  * AUTHORS: KARTHIK VENKATARAMANA PEMMARAJU & BHARATH RAMAGONI
  * DATE: 10/08/2017 - 10/13/2017
  * DESCRIPTION: Memory image of a linked list.
  ===========================================================================================
*/

#include <iostream>
#include "block.h"
using namespace std;

/**
  * @Type: class
  * @Data members: first - Pointer to the first Block in the list.
*/

class list{
  private:
    Block *first;
  public:
    void assign(Block *&firstBlock, int N);
    Block* unlink();
    void link(Block *);
    int size();
    void display();
};

/**
  * @Type: Function
  * @param: @firstBlock - The first Block in the list.
            @N - Number of Blocks in the list.
  * @desc: Appends N contiguous blocks to a list and sets the initial value to be 0.
*/
void list::assign(Block *&firstBlock, int N){
  first = firstBlock; // Catch first element into class variable.
  Block *nextBlock = firstBlock;
  for(int i = 1; i <= N; i++){
    nextBlock += sizeof(firstBlock); // Super Important! next value of Block in shared memory is stored contiguously.
    firstBlock -> data = 0; // Intial value is 0. (Optional!)
    firstBlock -> next = nextBlock;
    if(i == N)  // Last element of our linked list.
      firstBlock -> next = NULL;
    firstBlock = nextBlock;
  }
}

/**
  * @desc: Unlinks the first Block in the list and returns the pointer to it.
*/
Block* list::unlink(){
  Block *temp = first;
  if(first != NULL)
    first = first -> next;
  return temp;
}

/**
  * @desc: Links the given Block to the end of list
*/
void list::link(Block* newBlock){
  Block* temp = first;
  if(temp != NULL){ // if list is not empty
      while(temp -> next != NULL){ // traverse through the list
      temp = temp -> next;
    }
    temp -> next = newBlock;
  }
  else
    first = newBlock;
  newBlock -> next = NULL;
}
/**
  * @desc: returns the size of list.
*/
int list::size(){
  Block* temp = first;
  int size = 0;
  while(temp!= NULL){
    size++;
    temp = temp -> next;
  }
  return size;
}

/**
  * @desc: Displays the elements of list: (Mainly used for testing purposes. Ex: usage list1 -> display() in main.cpp file)
*/
void list::display(){
  Block* head = first;
  int size = 0;
  if(head == NULL)
    cout << "EMPTY LIST!";
  while(head!= NULL){
    cout << head -> data << "  " ;
    head = head -> next;
    size++;
  }
  cout << endl << "SIZE: " << size << endl;
}
