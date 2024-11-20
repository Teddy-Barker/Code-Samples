//This is the work of Teddy Barker
//RED ID: 825169794

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <getopt.h>
#include "pageTable.h"
#include "level.h"
#include "tracereader.h"
#include "log.h"
#include "tlb.h"

#define NORMAL_EXIT 1

using namespace std;

unsigned int* parseCommandLineArguments(int argc, char *argv[], int optind, unsigned int &levelCount);

int main (int argc, char *argv[]) {        
    //Variables for new command-line options
    int numAccesses = -1;
    int tlbSize = 0;
    string outputMode = "summary"; //Default output mode
    
    unsigned int* entryCount = nullptr; //Entry count to be used for the level sizes
    unsigned int levelCount = 0; //Level count to denote how many levels the page table tree will have

    //Parse command-line options
    int option;
    while ((option = getopt(argc, argv, "n:c:o:")) != -1) {
        switch (option) {
            case 'n': //Limit the number of memory accesses
                numAccesses = atoi(optarg); //Convert string argument to integer
                if (numAccesses <= 0) {
                    cerr << "Number of memory accesses must be a number, greater than 0.\n";
                    exit(NORMAL_EXIT);
                }
                break;
            case 'c': //TLB cache capacity
                tlbSize = atoi(optarg); //Convert string argument to integer
                if (tlbSize < 0) {
                    cerr << "Cache capacity must be a number, greater than or equal to 0.\n";
                    exit(NORMAL_EXIT);
                }
                break;
            case 'o': //Output mode
                outputMode = optarg;
                break;
            default:
                cerr << "Invalid argument\n";
                exit(NORMAL_EXIT);
        }
    }

    //Ensure that there are enough arguments for the trace file and level sizes
    if (optind >= argc || (argc - optind) < 2) {
        cerr << "Usage: " << argv[0] << " [options] <<tracefile>> <level sizes>.\n";
        exit(NORMAL_EXIT);
    }

    //Open the trace file
    FILE *tracef_h = fopen(argv[optind], "r");
    if (!tracef_h) {
        cerr << "Unable to open <<" << argv[optind] << ">>\n";
        exit(NORMAL_EXIT);
    }

    //Parse the page table level sizes starting from the next argument
    entryCount = parseCommandLineArguments(argc, argv, optind + 1, levelCount);
    
    //Check if total bits used in page table levels exceed 28
    int totalBits = 0;
    for (int i = 0; i < levelCount; i++) {
        totalBits += atoi(argv[optind + 1 + i]);
        if(atoi(argv[optind + 1 + i]) <= 0) {
            //Check if any of the levels are 0 or less
            cerr << "Level " << i << " page table must be at least 1 bit\n";
            exit(NORMAL_EXIT);
        }
    }
    if (totalBits > 28) {
        cerr << "Too many bits used in page tables\n";
        exit(NORMAL_EXIT);
    }


    //Create a new page table with the corresponding levels and entry counts
    PageTable pageTable = PageTable(levelCount, entryCount);

    //Helper variables used to handle the outputs
    unsigned int* bitMaskAry = pageTable.getBitMaskAry();  
    unsigned int* shiftAry = pageTable.getShiftAry();  
    unsigned int* pageIndices = new unsigned int[levelCount];  

    //Create the TLB if cache size is specified
    TLB* tlb = nullptr;
    if (tlbSize > 0) {
        tlb = new TLB(tlbSize);
    }

    //Handle output for bitmasks mode
    if (outputMode == "bitmasks") {
        log_bitmasks(levelCount, bitMaskAry);
        fclose(tracef_h);
        return 0; //End execution if we only need to print bitmasks
    }

    //Process the trace file
    p2AddrTr mtrace;
    unsigned int vAddr;
    int accessCount = 0;
    int tlbHits = 0;
    int pageTableHits = 0;

    while (NextAddress(tracef_h, &mtrace)) {
        vAddr = mtrace.addr;

        //Stop processing if the number of accesses is limited by -n N
        if (numAccesses != -1 && accessCount >= numAccesses) {
            break;
        }

        //Find the page indices based on the address and the masks
        for (int i = 0; i < levelCount; i++) {
            pageIndices[i] = pageTable.extractPageNumberFromAddress(vAddr, bitMaskAry[i], shiftAry[i]);
        }

        // Combine page indices across all levels to get the full VPN
        unsigned int vpn = 0;
        unsigned int shiftAmt = 0;
        for (int i = 0; i < levelCount; i++) {
            vpn = vpn << BIT_SIZE - shiftAry[i] - shiftAmt;
            vpn = vpn | pageIndices[i];
            shiftAmt = BIT_SIZE - shiftAry[i];
        }

        //Check the TLB first
        int pfn = -1;
        bool tlbHit = false;
        bool pageTableHit = false;

        if (tlb != nullptr) {
            pfn = tlb->lookup(vpn);
            if (pfn != -1) {
                tlbHit = true;
                tlbHits++;
            }
        }

        //If not found in TLB, check the page table
        if (pfn == -1) {
            pfn = pageTable.recordPageAccess(vAddr, pageTable.getRoot(), pageTableHit);
            if (pageTableHit == true) {
                pageTableHits++;
            }
            if (tlb != nullptr) {
                tlb->insert(vpn, pfn);  //Insert into TLB
            }
        }

        //Output results for va2pa_atc_ptwalk mode
        if (outputMode == "va2pa_atc_ptwalk") {
            unsigned int offset = vAddr & ((1 << shiftAry[levelCount - 1]) - 1);
            unsigned int physicalAddr = (pfn << shiftAry[levelCount - 1]) | offset;
            log_va2pa_ATC_PTwalk(vAddr, physicalAddr, tlbHit, pageTableHit);
        } else if (outputMode == "offset") {
            //Output the offset part of the virtual address
            unsigned int offset = vAddr & ((1 << shiftAry[levelCount - 1]) - 1);
            hexnum(offset);
        } else if (outputMode == "vpn2pfn") {
            //Output VPN to PFN mapping
            log_pagemapping(levelCount, pageIndices, pfn);
        } else if (outputMode == "va2pa") {
            //Output VA to PA mapping without TLB and page table lookup details
            unsigned int offset = vAddr & ((1 << shiftAry[levelCount - 1]) - 1);
            unsigned int physicalAddr = (pfn << shiftAry[levelCount - 1]) | offset;
            log_virtualAddr2physicalAddr(vAddr, physicalAddr);
        }

        accessCount++;
    }

    //Handle summary output mode
    if (outputMode == "summary") {
        unsigned int pageSize = 1 << shiftAry[levelCount - 1]; //Compute page size
        unsigned int framesUsed = pageTable.getFramesAllocated(); //Placeholder for frames used
        unsigned long int totalPageTableEntries = pageTable.getTotalPageTableEntries(); //Placeholder for page table entries

        log_summary(pageSize, tlbHits, pageTableHits, accessCount, framesUsed, totalPageTableEntries);
    }

    //Close the trace file
    fclose(tracef_h);

    //Free dynamic memory
    delete[] bitMaskAry;
    delete[] shiftAry;
    delete[] pageIndices;
    delete tlb;
    
    return 0;
}

//Method to parse the page table level sizes from multiple arguments
unsigned int* parseCommandLineArguments(int argc, char *argv[], int optind, unsigned int &levelCount) {
    levelCount = argc - optind; //The number of remaining arguments are the level sizes
    unsigned int* entryCount = new unsigned int[levelCount];

    for (int i = 0; i < levelCount; i++) {
        entryCount[i] = 1 << atoi(argv[optind + i]); //Convert each argument to an entry count (2^x)
    }

    return entryCount;
}
