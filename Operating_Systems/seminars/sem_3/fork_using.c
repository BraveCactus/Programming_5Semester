#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[])
{

	printf("argc: %d\n", argc);
	for(int i =0; i<argc; ++i)
		printf("argv[%d]: %s\n",i , argv[i]);

	char msg[] = "Hello";

	pid_t p = fork();

	if(p > 0)
	{
		printf("Parent: pid=%d; ppid=%d\n", getpid(), getppid());
		printf("Created child pid=%d\n", p);
		sleep(10);
	}
	else if(p == 0)
	{
		sleep(1);
		printf("Child: pid=%d; ppid=%d\n",getpid(),getppid());
		printf("Child msg: %s\n", msg);

		int rc = execlp("ls", "ls", "-a", "-l", NULL);
		if (rc == -1) printf("EXECL ERROR");
	}


	//sleep(10);

	printf("Finished...\n");
	return 0;
}
