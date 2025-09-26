#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
	char data[] = "ABC\n";
    // char* data2 = (char*)malloc(16);

    int len = sizeof(data);
    printf("lem = %d\n", len);

    int fd = open("test.txt",O_WRONLY|O_TRUNC|O_EXCL, 0664);
    perror("read open");
    if(fd == -1){
        // perror("open");
        return -1;
    }
    printf("FD: %d\n", fd);
    write(fd, data, len);

    close(fd);

    fd = open("test.txt", O_RDONLY, 0);
    if (fd == -1){
        perror("write open");
        return -1;
    }

    char buf[128];
    int n = read(fd, buf, sizeof(buf));
    buf[n] = '\0';

    printf("Read size: %d\n", n);
    printf("Read data %s\n", buf);
    return 0;
}