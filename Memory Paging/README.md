# Demand Paging with Address Translation Cache (ATC)

This program simulates multi-level demand paging with an Address Translation Cache (ATC), similar to the Translation Lookaside Buffer (TLB), in a **Linux environment**. It processes virtual memory addresses, implements address translation using a multi-level page table, and simulates TLB cache behavior with an approximation of the Least Recently Used (LRU) policy.

---

## Overview
The program simulates:
- Virtual to physical address translation.
- Multi-level page table structure.
- TLB caching for faster lookups.
- Demand paging to allocate physical frames dynamically.

### Key Features:
- Tracks TLB hits, page table hits, and page table misses.
- Handles memory references sequentially with configurable TLB size and page table levels.
- Outputs translation and performance statistics.

---

## Command-Line Arguments
1. **Mandatory Arguments**:
   - `<trace file>`: Path to the file containing memory trace data.
   - `<page table levels>`: List of integers specifying the number of bits for each page table level.

2. **Optional Arguments**:
   - `-n <N>`: Process the first N memory references (default: all references).
   - `-c <N>`: TLB cache capacity (default: 0, meaning no TLB).
   - `-o <mode>`: Output mode. Options:
     - `summary` (default): Displays performance stats.
     - `bitmasks`: Outputs bitmasks for each page table level.
     - `va2pa`: Shows virtual-to-physical address translations.
     - `vpn2pfn`: Displays virtual page numbers and frame numbers.

---

## Example Usage
```bash
# 2-level page table, process all addresses, output summary
./pagingwithatc trace.tr 8 12

# 3-level page table, process first 1000 addresses, output VPN-to-PFN mapping
./pagingwithatc -n 1000 -o vpn2pfn trace.tr 8 6 5

# 3-level page table with TLB caching, process 6400 addresses, output translations
./pagingwithatc -n 6400 -c 12 -o va2pa_atc_ptwalk trace.tr 8 6 10
