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
        execl("/bin/ls", "ls", "-l", NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    }
    else {
        wait(NULL);
        printf("This is parent process! \n");
        exit(EXIT_SUCCESS);
    }
}