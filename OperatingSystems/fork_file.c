//
// Created by Yaroslav on 13.03.2024.
//
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// https://www.csl.mtu.edu/cs4411.ck/www/NOTES/process/fork/create.html

int main() {
    // pid_t is a type that identifies the process and represents the processes ids. Actually it is signed int type
    // fork() is used to create a new process, which becomes the child process of the caller.
    // If the fork() executed successfully, then it will create two identical copies of address spaces, one of them for the parent process and the other is for the child process.
    // Since every process has its own address space, any modifications will be independent of the others.
    pid_t child_pid = fork();

    // We know that if fork returned a negative value, then the creation of the process was unsuccessful, so we have to return an error
    if (child_pid == -1) {
        perror("fork");  // perror prints the error to the stderr
        exit(EXIT_FAILURE);  // the program terminated with an error
    }

    // We also know that if fork returned a zero, then it returned it to the newly (successfully) created child process
    if (child_pid == 0) {
        printf("It is child process!\n");  // use stdout to print a message that this is a child process
        exit(EXIT_SUCCESS);  // the program terminated with the success
    } else if(child_pid > 0) {  // We also know that if fork returned a positive value, then it returned the processID of the child process to the parent
        wait((int *) 1);
        printf("It is parent process!\n");  // use stdout to print a message that this is a parent process
        exit(EXIT_SUCCESS);  // the program terminated with the success
    }
}