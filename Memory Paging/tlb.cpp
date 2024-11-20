//This is the work of Teddy Barker
//RED ID: 825169794

#include "tlb.h"
#include <iostream> 

//Constructor to initialize TLB of a given size
TLB::TLB(int size) {
    tlbSize = size;
    entries = new TLBEntry[size];
    global_lruCounter = 0;  //Initialize global LRU counter

    //Initialize TLB entries with invalid values
    for (int i = 0; i < size; i++) {
        entries[i].vpn = -1; //Invalid VPN
        entries[i].pfn = -1; //Invalid PFN
        entries[i].lruCounter = -1; //Not used yet
    }
}

//Destructor to clean up memory
TLB::~TLB() {
    delete[] entries;
}

//Lookup VPN in the TLB, return PFN or -1 if not found
int TLB::lookup(unsigned int vpn) {
    //Increment the gloval lru counter
    global_lruCounter++;
    for (int i = 0; i < tlbSize; i++) {
        if (entries[i].vpn == vpn) {
            //update lru counter since this entry was accessed
            entries[i].lruCounter = global_lruCounter;
            return entries[i].pfn; //Return the corresponding PFN
        }
    }
    return -1;  //TLB miss
}

//Insert new VPN -> PFN mapping into the TLB
void TLB::insert(unsigned int vpn, unsigned int pfn) {
    //Check if there's an empty slot in the TLB
    for (int i = 0; i < tlbSize; i++) {
        if (entries[i].vpn == -1) {
            //Insert in the empty slot
            entries[i].vpn = vpn;
            entries[i].pfn = pfn;
            entries[i].lruCounter = global_lruCounter;
            return;
        }
    }

    //If no empty slot, find the least recently used entry and replace it
    int lruIndex = 0;
    for (int i = 1; i < tlbSize; i++) {
        if (entries[i].lruCounter < entries[lruIndex].lruCounter) {
            lruIndex = i;
        }
    }

    //Replace the LRU entry
    replaceEntry(lruIndex, vpn, pfn);
}

//Replace an entry in the TLB using the LRU policy
void TLB::replaceEntry(int index, unsigned int vpn, unsigned int pfn) {
    entries[index].vpn = vpn;
    entries[index].pfn = pfn;
    entries[index].lruCounter = global_lruCounter;
}