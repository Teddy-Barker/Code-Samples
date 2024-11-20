/**
 * Teddy Barker
 */

#include <iostream>
#include "scheduler.h"

#define NORMAL_EXIT 0

using namespace std;



void* scheduler( void *ptr1) {
    // Shared data passed into the thread from the main thread
    SHARED_DATA* shared_data;
    shared_data = (SHARED_DATA*) ptr1;

    float alpha = shared_data->alpha;
    int turnaround = 0;

    float** predictions; // This float double pointer will be used for the 2D array logged for the predictions


    // Allocate memory for the float array
    predictions = new float*[shared_data->burstTimes.size()];

    for(int i = 0; i < shared_data->burstTimes.size(); i++) {
        predictions[i] = new float[shared_data->burstTimes[i].size()];
    }
    

    // Initializing Ready Queue and Blocked Queue and Complete Queue
    deque<PROCESS_DATA*> ready;
    deque<PROCESS_DATA*> blocked;
    deque<PROCESS_DATA*> complete;
    
    for(int i = 0; i < shared_data->burstTimes.size(); i++) {
        int temp_count = 0; // Used for the average 
        // Create a new Process instance
        PROCESS_DATA* newData = new PROCESS_DATA;
        newData->id = i;
        newData->executed_cpu = 0;
        newData->executed_io = 0;
        newData->turnaround = 0;
        newData->next_burst_index = 0;
        newData->prediction = 0;
        newData->last_burst = 0;
        
        for(int j = 0; j < shared_data->burstTimes[i].size(); j++) {
            newData->bursts.push_back(shared_data->burstTimes[i][j]);
            // If we're using exponential averaging, accumlate the sum for the purpose of setting the initial prediction to the average
            if(alpha != -1  && j % 2 == 0) {
                newData->prediction += shared_data->burstTimes[i][j];
                temp_count++;
            }

            // Otherwise, we'll use the actual burst as the first prediction
            if(alpha == -1) {
                newData->prediction = newData->bursts.front();
            }
        }

        // Complete the average by dividing
        if(temp_count != 0 && alpha != -1)
            newData->prediction /= temp_count;

        // Update the predictions list for the return output
        if(alpha != -1) {
            // For exponential averaging
            for(int i = 0; i < newData->bursts.size(); i++) {
                if(i == 0) {
                    newData->predictions.push_back(newData->prediction);
                } else if (i%2 == 1) {
                    newData->predictions.push_back(newData->bursts[i]);
                } else {
                    newData->predictions.push_back(-1);
                }
            }
        } else {
            // For not using exponential averaging
            for(int i = 0; i < newData->bursts.size(); i++) {
                newData->predictions.push_back(newData->bursts[i]);
            }
        }

        // Push the newly created process instance to the back of the list
        ready.push_back(newData);
    }
    
    for(int i = 0; i < ready.size(); i++) {
        unsigned int temp[ready[i]->bursts.size()];
        for(int j = 0; j < ready[i]->bursts.size(); j++) {
            temp[j] = ready[i]->bursts[j];
        }
        log_process_bursts(ready[i]->id, temp, ready[i]->bursts.size());
    }


    // Begin the simulation
    while(!ready.empty() || !blocked.empty()) {
        ExecutionStopReasonType stopReason;
        bool stopReasonValid = false; // This bool is important because it tells the program whether the stopReason variable needs to be processed.
        int quantum = 0;

        // Stable Sort the Ready Queue
        sort_ready(ready);

        // If the ready queue has at least one process, we simulate the cpu burst and handle io
        if(!ready.empty()) {
            quantum = cpuBurst(ready.front(), stopReason, turnaround);
            stopReasonValid = true;
        } else {
            //sort_blocked(blocked);
            quantum = blocked.front()->bursts[blocked.front()->next_burst_index];
            turnaround += quantum;
        }
        // Simulate the IO execution
        for(int i = 0; i < blocked.size(); i++) {
            // Update data
            if(blocked[i]->bursts[blocked[i]->next_burst_index] - quantum <= 0) {
                // Process is finished executing io and goes back to the ready queue
                blocked[i]->executed_io += blocked[i]->bursts[blocked[i]->next_burst_index];
                blocked[i]->bursts[blocked[i]->next_burst_index] = 0;
                blocked[i]->next_burst_index++;
                ready.push_back(blocked[i]);  
                update_prediction(blocked[i], alpha);
                blocked.pop_front();
                i--;
            } else { 
                // Process is not finished executing io and needs to update data accordingly
                blocked[i]->executed_io += quantum;
                blocked[i]->bursts[blocked[i]->next_burst_index] -= quantum;
            }
        }

        //cout << stopReason << endl;
        if(!ready.empty() && stopReasonValid) {
            // Handle the completion status of the executed process
            if(stopReason == COMPLETED) {
                // Push to the complete queue if its finished
                complete.push_back(ready.front());
                ready.pop_front();
            } else if (stopReason == ENTER_IO) {
                // Push to the blocked queue if its entering the io
                PROCESS_DATA* process = ready.front();
                ready.pop_front();
                blocked.push_back(process);
                sort_blocked(blocked);
            } else  if (stopReason == QUANTUM_EXPIRED){
                // Push to the back of the ready queue if the quantum has expired
                PROCESS_DATA* temp = ready.front();
                ready.pop_front();
                ready.push_back(temp);
                update_prediction(temp, alpha);
                sort_ready(ready);
            }
        }

    }

    // Log the complete processes
    for(int i = 0; i < complete.size(); i++) {
        int wait = complete[i]->turnaround - complete[i]->executed_cpu - complete[i]->executed_io;
        log_process_completion(complete[i]->id, complete[i]->turnaround, wait);
    }

    // Prepare the float double array
    for(int i = 0; i < complete.size(); i++) {
        for(int j = 0; j < complete[i]->predictions.size(); j++) {
            predictions[i][j] = complete[i]->predictions[j];
        }
    }

    // Log the predictions of all the processes in completed order
    for(int i = 0; i < complete.size(); i++) {
        log_process_estimated_bursts(complete[i]->id, predictions[i], complete[i]->predictions.size());
    }
    

    // Free the float double pointer
    for(int i = 0; i < shared_data->burstTimes.size(); i++) {
        delete[] predictions[i];
    }

    delete[] predictions;

    // Update busy waiting now that the simulation is complete
    shared_data->busyWaiting = false;
    pthread_exit(NORMAL_EXIT);
}

/**
 * This is a function to update the prediction, estimated or actual
 * 
 *  args: 
 *      - process is the PROCESS_DATA to update the prediction for
 *      - alpha is the alpha value used for exponential averaging
 * 
 *  */ 
void update_prediction(PROCESS_DATA* process, float alpha) {
        int index = process->next_burst_index;
        // If its current burst index is an IO execution 
        //  OR its the first cpu burst
        // return
        if(index == 0 || index % 2 == 1 ) {
            return;
        }
        // If alpha is -1 then we're not using exponential averaging
        else if (alpha == -1) {
            if(index < process->bursts.size()) {
                //cout << "**" << index;
                process->prediction = process->bursts[index];
                //process->predictions[index] = process->prediction;
            }
        }
        // Otherwise, calculate the exponential average for the next burst prediction
        else {
            double temp = process->predictions[process->next_burst_index-2];
            if(index-2 >= 0) {
                process->prediction = (alpha * process->last_burst) + ((1.0 - alpha) * temp);
                process->predictions[process->next_burst_index] = process->prediction;
            }
        }
}

/**
 * Implements an insertion sort which is stable and has a best case n time complexity
 * 
 * Meant to be used with the ready Queue because it sorts based on prediction
 */
void sort_ready(deque<PROCESS_DATA*> &process_list) {
    // For each element, move it down the sorted portion until its in ascending order
    for(int i = 1; i < process_list.size(); i++) {
        for(int j = i-1; j >= 0 && process_list[j + 1]->prediction < process_list[j]->prediction; j--) {
            // Swap processes if the left one is less than the right one
            PROCESS_DATA* temp = process_list[j+1];
            process_list[j+1] = process_list[j];
            process_list[j] = temp;
        }
    }
}

/**
 * Implements an insertion sort which is stable and has a best case n time complexity
 * 
 * Meant to be used with the blocked Queue because it sorts based on next io burst
 */
void sort_blocked(deque<PROCESS_DATA*> &process_list) {
    // For each element, move it down the sorted portion until its in ascending order
    for(int i = 1; i < process_list.size(); i++) {
        for(int j = i-1; j >= 0; j--) {
            // Swap processes if the left one is less than the right one
            if (process_list[j+1]->bursts[process_list[j+1]->next_burst_index] < process_list[j]->bursts[process_list[j]->next_burst_index]) {
                PROCESS_DATA* temp = process_list[j+1];
                process_list[j+1] = process_list[j];
                process_list[j] = temp;
            }
        }
    }
}


/**
 * This is a function to simulate a burst being executed by the CPU
 * 
 *  args: 
 *      - process is the process_data to update simulate the cput activty
 *      - stopReason is the pass by reference of the process state after 
 *          the burst
 * 
 *  return: 
 *      - int for the number of milliseconds this burst took
 *  */ 

int cpuBurst(PROCESS_DATA* process, ExecutionStopReasonType &stopReason, int &turnaround) {
    int burstLength = process->bursts[process->next_burst_index];

    // Update the process_data
    process->executed_cpu += burstLength;
    process->last_burst = process->bursts[process->next_burst_index];
    process->bursts[process->next_burst_index] = 0;
    turnaround += burstLength;

    // Save the stop reason into a variable
    if(process->next_burst_index + 1 >= process->bursts.size()) {
        process->turnaround = turnaround;
        stopReason = COMPLETED;
    } else if (process->bursts[process->next_burst_index] == 0) {
        // Increment the next burst index for future reference
        process->next_burst_index++;
        stopReason = ENTER_IO;
    } else {
        stopReason = QUANTUM_EXPIRED;
    }

    // Log the cpu burst
    log_cpuburst_execution(process->id, process->executed_cpu, process->executed_io, turnaround, stopReason);

    return burstLength;
}