#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

void signal_handler(int sig_num)
{
	printf("Got signal %d\n", sig_num);

	int status;
	pid_t p;
	while( (p = waitpid(-1, &status, WNOHANG)) > 0 )
	{
		printf("Child #%d:\n", p);

		if( WIFEXITED(status) )
			printf("self finished with code %d\n", WEXITSTATUS(status));
		else if ( WIFSIGNALED(status) )
			printf("killed by signal %d, core dumped = %d\n",
				 WTERMSIG(status), WCOREDUMP(status));
	}
}


int main()
{
	//signal(SIGCHLD, signal_handler);
	//signal(SIGQUIT, signal_handler);

	pid_t p = fork();
	if (p == 0)
	{
		sleep(1);
		return 123;
	}

	sleep(35);

	return 0;
}
