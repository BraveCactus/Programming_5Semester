#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

const int MAX_LINE_LENGTH = 256;
const int MAX_ARGUMENTS = 32;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    FILE* file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("Error: Cannot open file '%s'\n", argv[1]);
        return 1;
    }

    char line[MAX_LINE_LENGTH];
    int line_number = 0;

    printf("useless started\n");

    while (fgets(line, sizeof(line), file) != NULL) {
        line_number++;

        // Удаляем символ новой строки
        line[strcspn(line, "\n")] = '\0';
        line[strcspn(line, "\r")] = '\0';

        // Пропускаем пустые строки
        if (strlen(line) == 0) {
            continue;
        }
        
        char* delay_str = strtok(line, " \t");
        if (delay_str == NULL) {
            printf("Error: Missing delay on line %d\n", line_number);
            continue;
        }

        int delay = atoi(delay_str);
        if (delay < 0) {
            printf("Error: Negative delay on line %d\n", line_number);
            continue;
        }

        char* command = strtok(NULL, "");
        if (command == NULL) {
            printf("Error: Missing command on line %d\n", line_number);
            continue;
        }

        printf("Schedule: command '%s' will be executed after %d seconds\n", command, delay);

        pid_t pid = fork();
        
        if (pid == -1) {
            perror("Error: fork failed");
            continue;
        }

        if (pid == 0) {
            // ДОЧЕРНИЙ ПРОЦЕСС
            sleep(delay);
            printf("Command %s has been executed after %d seconds\n", command, delay);
            
            char* args[MAX_ARGUMENTS];
            int arg_count = 0;
            
            char command_copy[MAX_LINE_LENGTH];
            strcpy(command_copy, command);
            
            char* token = strtok(command_copy, " ");
            while (token != NULL && arg_count < MAX_ARGUMENTS - 1) {
                args[arg_count++] = token;
                token = strtok(NULL, " ");
            }
            args[arg_count] = NULL;
            
            execvp(args[0], args);

            perror("Error: execvp failed");
            exit(1);
        } else {
            // РОДИТЕЛЬСКИЙ ПРОЦЕСС
            printf("Start child process %d\n", pid);
        }   
    }

    fclose(file);

    printf("Waiting for all child processes to complete...\n");
    
    int status;
    pid_t child_pid;
    
    while ((child_pid = wait(&status)) > 0) {
        printf("Child process %d finished\n", child_pid);
    }

    printf("useless finished\n");
    return 0;
}