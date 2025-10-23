#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

int main()
{
	int fd = creat("_ttt", 0664);  // open(name, O_CREAT|O_WRONLY|O_TRUCT, 0664)
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

	char msg[] = "Hello-Hello";
	write(fd, msg, sizeof(msg)-1);

	off_t offset = 0;
	off_t pos = lseek(fd, offset, SEEK_END);
	printf("Current position: %ld\n", pos);

	//char msg2[] = "XYZ";
	//write(fd, msg2, sizeof(msg2)-1);

	printf("File has been created & resized & written to\n");

	close(fd);
	return 0;
}
