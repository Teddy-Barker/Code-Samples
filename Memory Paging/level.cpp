//This is the work of Teddy Barker
//RED ID: 825169794

#include "level.h"

#include <iostream>

using namespace std;

/*****************
** CONSTRUCTORS **
*****************/
Level::Level(unsigned int depth, unsigned int size, PageTable* pageTablePtr) {
    //Constructor for the Level that takes in the depth, size, and pointer to the page table object that manages the root level attribute
    if(size == 0)
        nextPtr = nullptr; //Do not allocate memory if the size should be 0
    else
        nextPtr = new Level* [size]; //Allocate memory for the sub levels, but the will start as nulltpr

    //Initialize all entries of the level array as nullptr, until they are required
    for(int i = 0; i < size; i++) {
        nextPtr[i] = nullptr;
    }

    //Update members
    this->depth = depth;
    this->pageTablePtr = pageTablePtr;
    this->size = size;

    pfn = 0; //Begin with no accesses
}