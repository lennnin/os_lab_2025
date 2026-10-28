#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
    pid_t pid = fork();
    if (pid == 0) {
        char **args = malloc((argc + 1) * sizeof(char*));
        args[0] = "./parallel_min_max";
        for (int i = 1; i < argc; i++) {
            args[i] = argv[i];
        }
        args[argc] = NULL;
        execv(args[0], args);
        perror("execv failed");
        exit(1);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            printf("Child process exited with status %d\n", WEXITSTATUS(status));
        }
    } else {
        perror("fork failed");
        return 1;
    }
    return 0;
}


