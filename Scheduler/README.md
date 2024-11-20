# Short-Term Process Scheduling Simulator

This program is a Linux-based simulator for short-term process scheduling, implementing the **Shortest Job Next (SJN)** strategy with exponential averaging for CPU burst prediction. It uses multi-threading with POSIX threads to simulate a uniprocessor environment.

---

## Overview
The program reads a list of processes and their burst times from an input file and simulates the SJN scheduling algorithm. It calculates turnaround times, wait times, and executes CPU and I/O bursts for each process. The results are printed to standard output.

Key features include:
- **Multi-threaded design**: Main thread handles input and spawns a scheduler worker thread.
- **Exponential averaging**: Predicts CPU bursts using a specified alpha value.
- **Process states**: Tracks Running, Blocked, and Ready states for each process.

---

## Command-Line Arguments
The program accepts the following arguments:

1. **Mandatory Argument**:
   - `<file>`: Path to the input file containing process bursts.

2. **Optional Argument**:
   - `-a <float>`: Specifies the alpha value (0.0 < alpha < 1.0) for exponential averaging. If omitted, the program uses actual burst times.

### Example Usage:
```bash
# Run the scheduler with actual burst times
./schedule bursts.txt

# Run the scheduler with alpha = 0.2 for exponential averaging
./schedule bursts.txt -a 0.2