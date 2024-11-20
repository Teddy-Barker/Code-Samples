//This is the work of Teddy Barker

#ifndef PAGETABLE_H
#define PAGETABLE_H

#include "level.h"

//BIT SIZE MACRO FOR THE MEMORY ADDRESSES:
#define BIT_SIZE 32

/*
 * Page Table class
 *   A descriptor containing the attributes of a N level page 
 *   table and a pointer to the root level (Level 0) object.
*/
class PageTable {
    public:
        PageTable(unsigned int levelCount, unsigned int* entryCount);

        unsigned int recordPageAccess(unsigned int address, Level * level, bool &flag);
        unsigned int extractPageNumberFromAddress(unsigned int address, unsigned intmask, unsigned int shift);

        Level* getRoot();
        unsigned int* getBitMaskAry();
        unsigned int* getShiftAry();
        unsigned int getFramesAllocated();
        unsigned long getTotalPageTableEntries();
        unsigned long countEntriesAtLevel(Level* level);


    private:
        unsigned int levelCount;
        unsigned int* bitMaskAry;
        unsigned int* shiftAry;
        unsigned int* entryCount;
        Level* root;
        unsigned int framesAllocated;
        unsigned int nextAvailablePFN;

        unsigned int bitwiseLog2(unsigned int num);
};

#endif