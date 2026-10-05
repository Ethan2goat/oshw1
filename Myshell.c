#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LINE 1024
#define MAX_ARGS 64

int main(void)
{
    char line[MAX_LINE];
    char *args[MAX_ARGS];

    while (1) {
        /* 1. Display the prompt */
        printf("myshell%% ");
        fflush(stdout);

        /* 2. Read the command the user typed (Ctrl-D also exits) */
        if (fgets(line, sizeof(line), stdin) == NULL) {
            printf("\n");
            break;
        }

        /* Remove the trailing newline */
        line[strcspn(line, "\n")] = '\0';

        /* Split the line into words: e.g. "ls -l" -> args[0]="ls", args[1]="-l" */
        int argc = 0;
        char *token = strtok(line, " \t");
        while (token != NULL && argc < MAX_ARGS - 1) {
            args[argc++] = token;
            token = strtok(NULL, " \t");
        }
        args[argc] = NULL;            /* exec needs a NULL-terminated list */

        if (argc == 0)                /* user just pressed Enter */
            continue;

        /* 3. Built-in command: exit */
        if (strcmp(args[0], "exit") == 0)
            break;

        /* 4. Fork a child process */
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork failed");
        }
        else if (pid == 0) {
            /* Child: replace itself with the requested command */
            execvp(args[0], args);
            /* Only reached if exec fails */
            perror("exec failed");
            exit(1);
        }
        else {
            /* Parent: wait for the child to finish, then show prompt again */
            waitpid(pid, NULL, 0);
        }
    }

    return 0;
}