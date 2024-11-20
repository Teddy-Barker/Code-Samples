//This is the work of Teddy Barker

#ifndef LEVEL_H
#define LEVEL_H

class PageTable;

/*
 * Level class
 *   An entry for an arbitrary level, this is the structure which 
 *   represents a node of one of the levels in the page tree/table.  
 *   It manages a pointer to another level as a double pointer.
*/
class Level {
    public:
        unsigned int depth;
        unsigned int pfn;

        Level(unsigned depth, unsigned int size, PageTable* pageTablePtr);

        Level** nextPtr;
        PageTable* pageTablePtr;
        unsigned int size;
};

#endif