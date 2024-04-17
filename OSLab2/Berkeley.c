//
// Created by Yaroslav on 17.04.2024.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

#define NUM_SLAVES 3    // Number of slave processes, which for this program is 3

// Shared data (memory) struct definition
struct shared_data {
    time_t value_of_time;  // Variable of the time value
    int correction_value;     // Variable of the correction value
};

// Function to generate a random time value
time_t generate_pseudo_random_time() {
    time_t pseudo_rand_time = rand() % 100 + time(NULL);  // Pseudo-random time from now to 100 seconds within
    return pseudo_rand_time;  // return the pseudo-random time
}

// Function for the slave processes
void slave_processes(int shmid, int slave_id) {  // shmid is shared memory segment id, and slave_id is slave id
    // Getting access to the shared memory
    struct shared_data * shmid_correct_type = shmat(shmid, NULL, 0);  // Converting to the required type
    struct shared_data *shared_memory = shmid_correct_type;  // Set the required value
    if (shared_memory == (void *) -1) {  // Check if shared_memory attached or no
        perror("Error: shmat");  // shmat is shared memory attached
        exit(EXIT_FAILURE);
    }
    time_t pseudo_random_time = generate_pseudo_random_time();  // generate a pseudo-random time
    shared_memory[slave_id].value_of_time = pseudo_random_time;  // adding it to the shared memory
    // Wait for master to calculate corrections
    while (shared_memory[NUM_SLAVES].correction_value == 0) { // here we have to wait the "master" process in order to
        // compute all the corrections. Actually simulation with the "sleep(3);" value
        sleep(3);
    }
    pseudo_random_time += shared_memory[NUM_SLAVES].correction_value;  // adding the corrections

    printf("Slave %d: time before adjustment: %s", slave_id, asctime(localtime(&shared_memory[slave_id].value_of_time)));  // clock corrections
    printf("Slave %d: time after adjustment: %s", slave_id, asctime(localtime(&pseudo_random_time)));  // clock corrections
    if (shmdt(shared_memory) == -1) {  // here we are detaching from the shared memory
        perror("Error: shmdt");  // raising an error; shmdt is shared memory detach.
        exit(EXIT_FAILURE);
    }
}

// Function for the master process
void master_process(int shmid) {  // shmid is shared memory segment id
    // Getting access to the shared memory
    struct shared_data * shmid_correct_type = shmat(shmid, NULL, 0);  // Converting to the required type
    struct shared_data *shared_memory = shmid_correct_type;   // Set the required value
    if (shared_memory == (void *) -1) {  // Check if shared_memory attached or no
        perror("Error: shmat");   // shmat is shared memory attached
        exit(EXIT_FAILURE);
    }
    time_t total_time = 0;  // total time initialization
    time_t max_time_diff = 0;  // time difference initialization
    for (int i = 0; i < NUM_SLAVES; i++) {
        total_time += shared_memory[i].value_of_time;  // adding time of all slaves to the total time
    }
    time_t avg_time = total_time / NUM_SLAVES;  // average time calculation
    // Calculate corrections and find maximum time difference
    for (int i = 0; i < NUM_SLAVES; i++) {
        shared_memory[i].correction_value = difftime(avg_time, shared_memory[i].value_of_time);  // corrections calculation
        if (abs(shared_memory[i].correction_value) > max_time_diff) {  // here we check if the corrections more than our max_time_diff
            max_time_diff = abs(shared_memory[i].correction_value);  // and update the value if it is, in other words
            // calculating the maximum time difference in all slaves
        }
    }
    shared_memory[NUM_SLAVES].correction_value = max_time_diff;  // here we set the correction value for the master.
    // It must be equal to the maximum time difference
    if (shmdt(shared_memory) == -1) {  // again, here we are detaching the shared memory
        perror("Error: shmdt");
        exit(EXIT_FAILURE);
    }
}

int main() {
    int shmid;  // initializing shmid
    key_t key = ftok("/tmp", 'a');

    if ((shmid = shmget(key, (NUM_SLAVES + 1) * sizeof(struct shared_data), IPC_CREAT | 0666)) < 0) {  // shared memory
        // segment creation with error if it was not create
        perror("Error: shmget");
        exit(EXIT_FAILURE);
    }

    // Initialize shared memory
    struct shared_data * shmid_correct_type = shmat(shmid, NULL, 0);  // Converting to the required type
    struct shared_data *shared_memory = shmid_correct_type;   // Set the required value
    if (shared_memory == (void *) -1) {  // Check if shared_memory attached or no
        perror("Error: shmat");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < NUM_SLAVES; i++) {
        pid_t pid = fork();  // fork process (as it was in the first assignment) for the slaves
        if (pid < 0) {
            perror("Error: fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {  // check if this is child process
            slave_processes(shmid, i);  // this is child process
            exit(EXIT_SUCCESS);
        }
    }
    master_process(shmid);  // this is master process
    for (int i = 0; i < NUM_SLAVES; i++) {  // here we are terminating all child processes
        wait(NULL);
    }
    if (shmdt(shared_memory) == -1) {  // detaching the shared_memory
        perror("Error: shmdt");
        exit(EXIT_FAILURE);
    }
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {  // removing shared memory
        perror("Error: shmctl");
        exit(EXIT_FAILURE);
    }
    return 0;
}
