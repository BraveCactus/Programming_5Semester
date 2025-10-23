#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
	pid_t pid = getpid();
	pid_t ppid = getppid();
	printf("My PID: %d; PPID: %d\n", pid, ppid);

	printf("argc: %d\n", argc);
	for(int i =0; i<argc; ++i)
		printf("argv[%d]: %s\n",i , argv[i]);


	//sleep(10);

	printf("Finished...\n");
	return 0;
}
