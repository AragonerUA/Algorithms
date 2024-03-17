//
// Created by Yaroslav on 13.03.2024.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    pid_t child_pid = fork();

    if (child_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (child_pid == 0) {
        execl("/bin/ls", "ls", "-l", NULL);  // This function is used to replace the currently running process with another process. The PID of the process stays unchanged
        perror("execl");  // perror prints the error to the stderr
        exit(EXIT_FAILURE);  // the program terminated with an error
    }
    else {
        int parent_process_status;  // initialize the variable that will store the status of the parent process
        // pid_t again means the same
        pid_t terminated_child_pid = waitpid(child_pid, &parent_process_status, 0);  // waitpid() command is used to suspend the calling process until the child process ends or is stopped
        // in our case we have the 'child_pid' - child process that the caller wants to wait for
        // in our case 'parent_process_status' is a place where waitpid() can store a value

        // again, if we have the negative response than we got unsuccessful suspend (work of a waitpid process)
        if (terminated_child_pid == -1) {
            perror("waitpit");
            exit(EXIT_FAILURE);
        }

        // check if the child exited normally
        if (WIFEXITED(parent_process_status)) {
            printf("Child process %d exited with the next status %d\n", terminated_child_pid, WEXITSTATUS(parent_process_status));  // and return the status of the response
        }
        // if the child terminated
        else if (WIFSIGNALED(parent_process_status)) {
            printf("Child process %d terminated by signal %d\n", terminated_child_pid, WTERMSIG(parent_process_status));  // This macro queries the termination status of a child process to determine which signal caused the child process to exit.
        }
        // If nothing was returned from the previous queries
        printf("This is parent process!\n");
        exit(EXIT_SUCCESS);
    }
}