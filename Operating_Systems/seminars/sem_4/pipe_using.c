#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[], char* envp[])
{
    int fd[2];
    if( pipe(fd) == -1 ){ perror("pipe"); return -1; }

    pid_t p = fork();

    if(p > 0){
        close(fd[0]); // родитель не должен читать
        char data[] = "Hello world";
        write(fd[1], data, sizeof(data));
    }

    if (p == 0){
        close(fd[1]); // ребенок не должен писАть
        char buf[128];
        read(fd[0], buf, sizeof(buf));
        printf("Read pipe: %s\n", buf);
    }

    close(fd[0]);
    close(fd[1]);

    return 0;
}