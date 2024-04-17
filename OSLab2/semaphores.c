#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_READERS 7  // number of readers, which for this program is 7
#define NUM_WRITERS 3  // number of writers, which for this program is 3
sem_t read_mutex, write_mutex;  // these values are semaphore values
int number_of_readers = 0;  // tracking the number of the active readers

_Noreturn void *reader(void *arg) {  // noreturn function reader
    while (1) {
        sem_wait(&read_mutex);  // waiting to be accessed to the number_of_readers
        number_of_readers++;  // when the access was gained, increment the number of active users
        if (number_of_readers == 1) {  // if we have active readers, then we do not give access to writer, making it wait
            sem_wait(&write_mutex);
        }
        sem_post(&read_mutex);  // giving access to the readers

        // Shared resources. Here we are reading shared resources
        printf("Reader %ld is reading.\n", (long)arg);
        sleep(5); // This line is optional, but I've used it for better visualization process.
        // Actually here is the simulation of the reading process (actual reading process may last a time)
        sem_wait(&read_mutex);  // Here we are waiting for the access to the readers
        number_of_readers--;  // When the reader finished, remove it from the active readers
        if (number_of_readers == 0) {  // If all the readers finished (actually if last reader finished),
            // then we have access to the writers
            sem_post(&write_mutex);
        }
        sem_post(&read_mutex);  // Giving access to the readers
    }
}

_Noreturn void *writer(void *arg) {  // noreturn function writer
    while (1) {
        sem_wait(&write_mutex);  // waiting to be accessed to the resources

        // Shared resources. Here we are writing shared resources
        printf("Writer %ld is writing.\n", (long)arg);
        sleep(7); // This line is optional, but I've used it for better visualization process.
        // Actually here is the simulation of the writing process (actual writing process may last a time)

        sem_post(&write_mutex);  // Giving access to the resources
    }
}

int main() {
    pthread_t readers[NUM_READERS], writers[NUM_WRITERS];  // Threads

    // Initializing process
    sem_init(&read_mutex, 0, 1);  // Initialization the reader semaphore
    sem_init(&write_mutex, 0, 1);  // Initialization the writer semaphore

    // Creating reader threads
    for (long i = 0; i < NUM_READERS; i++) {
        pthread_create(&readers[i], NULL, reader, (void *)i);
    }

    // Creating writer threads
    for (long i = 0; i < NUM_WRITERS; i++) {
        pthread_create(&writers[i], NULL, writer, (void *)i);
    }

    // Adding more reader threads
    for (int i = 0; i < NUM_READERS; i++) {
        pthread_join(readers[i], NULL);
    }

    // Adding more writer threads
    for (int i = 0; i < NUM_WRITERS; i++) {
        pthread_join(writers[i], NULL);
    }

    // Adding more reader threads
    for (int i = 0; i < NUM_READERS; i++) {
        pthread_join(readers[i], NULL);
    }

    // Adding more writer threads
    for (int i = 0; i < NUM_WRITERS; i++) {
        pthread_join(writers[i], NULL);
    }

    sem_destroy(&read_mutex);  // Destroying read semaphore
    sem_destroy(&write_mutex);  // Destroying write semaphore

    return 0;
}
