#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>

typedef struct dirent _dirent;

int main(int argc, char* argv[])
{    
    DIR *dir;
    struct dirent *de;    
    
    dir = opendir(".");
    if (dir == NULL) {
        perror("opendir");
        return 1;
    }
    
    while ((de = readdir(dir)) != NULL) { 
        char type = '?';
        switch(de->d_type){
            case DT_REG: type = '-'; break;
            case DT_DIR: type = 'd'; break;
            default: type = '~'; break;
        }
        printf("%c %s\n",type, de->d_name); 

        if (type == 'd'){
            char sub_dir_name[256];
            sub_dir_name = dir_name + "/" + de->d_name;
            files_list();
        }
    }
    
    closedir(dir);
    return 0;
}