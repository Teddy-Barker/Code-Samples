/**
 * Teddy Barker
 */

#include <iostream>
#include <vector>
#include <fstream>
#include <stdio.h>
#include <pthread.h>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <getopt.h>
#include <cstring>
#include <cerrno>
#include "scheduler.h"

#define NORMAL_EXIT 0

using namespace std;

struct COMMAND_LINE_ARGS {
    int argc;
    char** argv;
};

void* read_and_process( void *ptr1);

// Main function calls the bulky read and process function and passes the command line arguments
int main(int argc, char** argv) {
    COMMAND_LINE_ARGS args = {argc, argv};

    read_and_process(&args);

    return NORMAL_EXIT;
}

/**
 * This is a function that reads and processes and input file, and 
 * creates a worker scheduler thread that simulates a series of cpu
 * and io bursts.
 * 
 *  args: 
 *      - ptr1 is meant to be the COMMAND_LINE_ARGS
 * 
 *  */ 
void* read_and_process( void *ptr1)
{
    //Processes the command line arguments
    COMMAND_LINE_ARGS* args = (COMMAND_LINE_ARGS*) ptr1;
    int argc = args->argc;
    char** argv = args->argv;

    const char *file_path = nullptr;
    float alpha = -1.0; // Default value indicating alpha is not set
    int option;

    // Parsing command line arguments using getopt
    while ((option = getopt(argc, argv, "a:")) != -1) {
        switch (option) {
            case 'a': // Optional alpha argument
                alpha = atof(optarg);
                if (alpha <= 0.0 || alpha >= 1.0) {
                    cout << "Alpha for exponential averaging must be within (0.0, 1.0)" << endl;
                    exit(NORMAL_EXIT);
                }
                break;
            case '?': // Unrecognized option
                exit(NORMAL_EXIT);
                break;
        }
    }

    // After processing all option arguments, remaining arguments are non-option
    if (optind < argc) {
        file_path = argv[optind]; // The first non-option argument is the file path
    }

    // Abort message for incorrect command line signature
    if (!file_path) {
        cerr << "Usage: " << argv[0] << " [-a alpha] <input_file>" << endl;
        exit(NORMAL_EXIT);
    }

    // Check file existence and ability to open
    ifstream file(file_path);
    if (!file.is_open()) {
        // Error handling for enable to open
        cerr << "Unable to open <<" << file_path << ">>" << endl;
        exit(NORMAL_EXIT);
    }
    

    vector<vector<int>> burstTimes;
    string line;

    // Parse the input file into a 2D vector
    while (getline(file, line)) {
        stringstream ss(line);
        int burst;
        vector<int> bursts;

        while (ss >> burst) {
            // Error hanlding for the burst being not greater than 0
            if(burst <= 0) {
                cerr << "A burst number must be bigger than 0" << endl;
                exit(NORMAL_EXIT);
            }
            bursts.push_back(burst);
        }
        // Error handling for the burst list being even
        if(bursts.size() % 2 == 0) {
            cerr << "There must be an odd number of bursts for each process" << endl;
            exit(NORMAL_EXIT);
        }
        burstTimes.push_back(bursts);
    }

    // Create a worker thread for the scheduler to perform all of its simulations
    pthread_attr_t pthread_attr_default;
    pthread_t worker_thread;

    SHARED_DATA shared_data = {burstTimes, alpha, true};

    pthread_attr_init(&pthread_attr_default);
    pthread_create( &worker_thread, &pthread_attr_default, &scheduler, &shared_data);
    

    while(shared_data.busyWaiting) {
        // Do nothing while its busy waiting
    }
    file.close();

    pthread_exit(NORMAL_EXIT);
}