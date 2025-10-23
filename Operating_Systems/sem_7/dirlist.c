#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

// typedef  struct dirent  _dirent;

void dir_list(char* name)
{
	printf("--- %s:\n", name);

	DIR* dir = opendir(name);
	if( ! dir ){
		perror("opendir");
		return;
	}

	struct dirent* de = NULL;
	while( de = readdir(dir) )
	{
		char full_name[256];
		sprintf(full_name, "%s/%s", name, de->d_name);		
		
		struct stat ifo;
		if( stat(full_name, &ifo) == -1 )
		{
			perror("stat()");
			continue;
		}

		char type = '~';
		switch(ifo.st_mode & S_IFMT)
		{
			case S_IFREG:  type='-';  break;
			case S_IFDIR:  type='d';  break;
			default:       type='?';
		}

		printf("%c %s\n", type, de->d_name);

		if( type == 'd' && strcmp(de->d_name, ".")!=0 && strcmp(de->d_name, "..")!=0 ){
			dir_list(full_name);
		}
	}

	closedir(dir);
}

int main(int argc, char* argv[])
{
	if( argc != 2 ){
		printf("usage: %s DIR_NAME\n", argv[0]);
		return 1;
	}

	dir_list(argv[1]);

	return 0;
}
