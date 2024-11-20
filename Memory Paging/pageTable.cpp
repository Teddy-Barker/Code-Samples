//This is the work of Teddy Barker
//RED ID: 825169794

#include "pageTable.h"
#include "level.h"
#include <iostream>

using namespace std;

/*****************
** CONSTRUCTORS **
*****************/
PageTable::PageTable(unsigned int levelCount, unsigned int* entryCount) {
    //Setting members to paramater values
    this->levelCount = levelCount;
    this->entryCount = new unsigned int[levelCount];
    for(int i = 0; i < levelCount; i++)
        this->entryCount[i] = entryCount[i];


    //Build a Shift Array corresponding to the # of bits to shift for getting level i page index

    unsigned int bitSizeTemp = BIT_SIZE; //Helper variable to start at BIT_SIZE and count the shifts necessary for i level

    shiftAry = new unsigned int[levelCount]; //Allocate shiftAry with size equal to the amount of levels

    //Create the shiftAry elements
    for(int i = 0; i < levelCount; i++) {
        unsigned int temp = bitwiseLog2(entryCount[i]); //Helper variable for the exponent part of the entryCount

        shiftAry[i] = bitSizeTemp - temp;
        bitSizeTemp -= temp;
    }

    //Build a Bitmask Array corresponding to the entryCount
    bitMaskAry = new unsigned int[levelCount];
    for(int i = 0; i < levelCount; i++) {
        //Build a bitmask for the given level (entryCount[i])
        bitMaskAry[i] = 0;

        //Helper variable for the exponent part of the entryCount
        unsigned int temp = bitwiseLog2(entryCount[i]); 
        // 1. For range between 0 and entryCount[i]
        for(int j = 0; j < temp; j++) {
        // 2. Right shift 1
            bitMaskAry[i] = bitMaskAry[i] << 1;
        // 3. OR with 1
            bitMaskAry[i] = bitMaskAry[i] | 1;
        }
        // 4. Left shift shiftAry[i]
            bitMaskAry[i] = bitMaskAry[i] << shiftAry[i];
    }

    //Allocating new root level
    root = new Level(0, entryCount[0], this);

    for(int i = 0; i < entryCount[0]; i++) {
        root->nextPtr[i] = nullptr;
    }

    //Initializing the starting pfn and frames allocated to 0
    nextAvailablePFN = 0;
    framesAllocated = 0;
}

/************
** METHODS **
************/
/**
 * This is the bread and butter.
 * Returns the integer for the physical frame number for this vpn
 *  it also tracks pagetable hit or miss using the flag
 */
unsigned int PageTable::recordPageAccess(unsigned int address, Level* level, bool &flag) {
    //Temporary int masked with the first bitmask
    unsigned int masked = address & bitMaskAry[level->depth];

    //Right shift to find offset on this level
    masked = masked >> shiftAry[level->depth];

    if(level->depth == levelCount - 1) {
        //This is the leaf level, so handle the PFN assignment here
        if(level->nextPtr[masked] == nullptr) {
                //Create a new leaf node and assign the next available PFN
                level->nextPtr[masked] = new Level(level->depth + 1, 0, this);

                //Store the PFN at the leaf node by using the next available PFN
                level->nextPtr[masked]->pfn = nextAvailablePFN++; 
                //Update pagetable hit flag
                flag = false;

                //Record the Newly allocated fram
                framesAllocated++;
                //Return the pfn
                return level->nextPtr[masked]->pfn;

        }
        //Update pagetable hit flag
        flag = true;
        //Return the PFN stored in this leaf node
        return level->nextPtr[masked]->pfn;
    } else if(level->nextPtr[masked] != nullptr) {
        //Continue to the next level
        return recordPageAccess(address, level->nextPtr[masked], flag);
    } else {
        //Add the next level
        level->nextPtr[masked] = new Level(level->depth + 1, entryCount[level->depth + 1], this);
        return recordPageAccess(address, level->nextPtr[masked], flag);
    }
}

/**
 * Returns the vpn based upon a given address and masking style
 */
unsigned int PageTable::extractPageNumberFromAddress(unsigned int address, unsigned intmask, unsigned int shift) {
    //Temporary int masked with the first bitmask
    unsigned int masked = address & intmask;

    //Right shift to find offset on this level
    masked = masked >> shift;

    return masked;
}

/**
 * Getter for the total page table entries
 */
unsigned long PageTable::getTotalPageTableEntries() {
    return countEntriesAtLevel(root);  //Start counting from the root
}

/**
 * Returns the entry count at this level and all levels below it
 */
unsigned long PageTable::countEntriesAtLevel(Level* level) {
    if (level == nullptr) return 1;

    //Count the level, not the root
    int count = 0;
    if(level != root)
        count = 1;
    
    //Recursively count entries in the next levels
    for (unsigned int i = 0; i < level->size; i++) {
        count += countEntriesAtLevel(level->nextPtr[i]);
    }
    return count;
}

/**
 * Helper Function for a bitwise log base 2
 */
unsigned int PageTable::bitwiseLog2(unsigned int num) {
    //Implement a makeshift log2 using left shifting
        unsigned int counter = 0; //Helper variable to remember how many left shifts (This will be the result of log2(num))

        while(num > 1) {
            //While the power is greater than 2^0 (which is 1), increment counter and left shift power
            num = num >> 1;
            counter++;
        } 
    return counter;
}

/**
 * Getter for Bit mask array
 */
unsigned int* PageTable::getBitMaskAry() {
    return bitMaskAry;
}

/**
 * Getter for Shift Array
 */

unsigned int* PageTable::getShiftAry() {
    return shiftAry;
}

/**
 * Getter for root level
 */
Level* PageTable::getRoot() {
    return root;
}

/**
 * Getter for frames allocated
 */
unsigned int PageTable::getFramesAllocated() {
    return framesAllocated;
}