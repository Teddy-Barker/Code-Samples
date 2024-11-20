//This is the work of Teddy Barker

#ifndef TLB_H
#define TLB_H

/**
 * This Struct mimics a TLB entry and manages three attributes:
 *  - virtual page number
 *  - physical frame number
 *  - least recently used counter
 */
struct TLBEntry {
    unsigned int vpn;
    unsigned int pfn;
    int lruCounter;
};

/**
 * This class is a Translation Lookaside Buffer and manages three attributes:
 *  - tlb size
 *  - global least recently used counter
 *  - list of TLB Entries
 */
class TLB {
public:
    TLB(int size);
    ~TLB();
    int lookup(unsigned int vpn);
    void insert(unsigned int vpn, unsigned int pfn);

private:
    TLBEntry* entries;
    int tlbSize;
    int global_lruCounter;
    void replaceEntry(int index, unsigned int vpn, unsigned int pfn);
};

#endif