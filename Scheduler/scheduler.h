/**
 * Teddy Barker
 */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <iostream>
#include <vector>
#include <deque>
#include <fstream>
#include <stdio.h>
#include <pthread.h>
#include <string>
#include <fstream>
#include "log.h"

using namespace std;

struct SHARED_DATA {
    vector<vector<int>> burstTimes; // The list of bursts of all the processes
    float alpha;
    bool busyWaiting;
};

struct PROCESS_DATA {
    int id; // Process ID that aligns with the index of the input file
    vector<int> bursts; // The remaining actual bursts of the process
    vector<float> predictions; // The predictions of the exponential averaging
    int executed_cpu; // The executed CPU bursts so far during the scheduling process
    int executed_io; // The executed I/O bursts so far during the scheduling process
    int turnaround; // The turnaround (or completion) time of the process.
    float prediction; // The prediction based upon either exponential averaging or next burst
    int last_burst; // The last actual cpu burst
    int next_burst_index; // The index of the next burst that needs to be performed
};

void* scheduler( void *ptr1);

void update_prediction(PROCESS_DATA* process, float alpha);

void sort_ready(deque<PROCESS_DATA*> &process_list);

void sort_blocked(deque<PROCESS_DATA*> &process_list);

int cpuBurst(PROCESS_DATA* process, ExecutionStopReasonType &stopReason, int &turnaround);

#endif