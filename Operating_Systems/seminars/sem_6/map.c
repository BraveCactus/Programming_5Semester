#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>

int main()
{
	// umask(0000);
	//int fd = creat("_ttt", 0664);
	int fd = open("_ttt", O_CREAT|O_RDWR|O_TRUNC, 0664);
	if( fd < 0 ){
		perror("creat");
		return 1;
	}

	size_t len = 123;
	int rc = ftruncate(fd, len);
	if( rc < 0 ){
		perror("ftruncate");
		return 2;
	}

	//size_t mlen = 3*sizeof(int);
	size_t line_len = 16;
	size_t mlen = 3*line_len;
	void* mem = mmap(
		NULL,                    // start address, auto choice
	    mlen,                    // length
		PROT_READ | PROT_WRITE,  // protection -> access rights
		MAP_SHARED,              // flags
		fd,
		0                        // offset
	);

	if( mem == MAP_FAILED ){
		perror("mmap");
		return 3;
	}
	close(fd);


	//int* C = (int*)mem;
	//C[0] = 123;
	//C[1] = 456;
	//C[2] = 789;
	char* lines = (char*)mem;
	strcpy(lines + (0*line_len), "Hello1");
	strcpy(lines + (1*line_len), "Hello2");
	strcpy(lines + (2*line_len), "Hello3");

	munmap(mem, mlen);
	return 0;
}
