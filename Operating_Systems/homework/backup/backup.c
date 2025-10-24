#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <errno.h>

static int CreateDirectoryIfNotExists(const char* path) {
    struct stat st;
    // Проверяем существование директории
    if (stat(path, &st) == -1) {
        // Директория не существует, создаем её
        if (mkdir(path, 0755) != 0) {            
            fprintf(stderr, "Error: creation directory '%s' failed: %s\n", path, strerror(errno));
            return -1;
        }
        printf("Directory created: %s\n", path);
    }
    return 0;
}

int NeedBackup(const char* src_path, const char* dst_path) {
    struct stat src_stat, dst_stat;
    
    // Получаем информацию об исходном файле
    if (stat(src_path, &src_stat) != 0) {
        fprintf(stderr, "Warning: access to source file failed '%s': %s\n", src_path, strerror(errno));
        return 0; // Пропускаем файл если не можем прочитать
    }
    
    // Проверяем существование файла backup
    if (stat(dst_path, &dst_stat) != 0) {
        if (errno == ENOENT) {
            return 1; // Backup не существует - нужен
        }
        fprintf(stderr, "Warning: access to backup file failed '%s': %s\n", dst_path, strerror(errno));
        return 0;
    }
    
    // Сравниваем время последнего изменения
    if (src_stat.st_mtime > dst_stat.st_mtime) {
        return 1; // Исходный файл новее - нужен backup
    }
    
    return 0; // Backup актуален - не нужен
}

int BackupFile(const char *source_file, const char *dest_file) {
    printf("Creating backup: %s -> %s\n", source_file, dest_file);
    
    // Открываем исходный файл в режиме чтения двоичных данных
    FILE *src_file = fopen(source_file, "rb");
    if (!src_file) {
        fprintf(stderr, "Error: opening source file failed '%s': %s\n", source_file, strerror(errno));
        return 1;
    }

    // Открываем целевой файл в режиме записи двоичных данных
    FILE *dst_file = fopen(dest_file, "wb");
    if (!dst_file) {
        fclose(src_file);
        fprintf(stderr, "Error: opening backup file failed '%s': %s\n", dest_file, strerror(errno));
        return 1;
    }

    // Буфер для чтения данных размером 4KB
    char buffer[4096];
    size_t bytes_read;
    
    // Чтение данных из исходного файла до конца
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), src_file)) > 0) {
        // Запись прочитанных данных в целевой файл
        size_t bytes_written = fwrite(buffer, 1, bytes_read, dst_file);
        if (bytes_written != bytes_read) {
            fprintf(stderr, "Error: writing data to backup file failed '%s'\n", dest_file);
            fclose(src_file);
            fclose(dst_file);
            return 1;
        }
    }

    // Закрываем файлы
    fclose(src_file);
    fclose(dst_file);

    // Создаем дочерний процесс для сжатия файла
    pid_t pid = fork();
    if (pid < 0) {
        fprintf(stderr, "Error: forking failed: %s\n", strerror(errno));
        return 1;
    }

    // В дочернем процессе выполняем сжатие
    if (pid == 0) {
        // Запускаем gzip для сжатия файла
        execlp("gzip", "gzip", "-f", dest_file, NULL);
        // Если execlp вернул управление - произошла ошибка
        fprintf(stderr, "Error: squeezing file error '%s': %s\n", dest_file, strerror(errno));
        exit(1);
    }

    // Родительский процесс ждет завершения сжатия
    int status;
    waitpid(pid, &status, 0);
    
    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
        printf("File has been squeezed: %s.gz\n", dest_file);
    } else {
        fprintf(stderr, "Error: squeezing of '%s' failed\n", dest_file);
        return 1;
    }

    return 0;
}

int BackupFolder(const char *source_folder, const char *dest_folder) {
    // Создаем целевую директорию если её нет
    if (CreateDirectoryIfNotExists(dest_folder) != 0) {
        return 1;
    }

    // Открываем исходную директорию для чтения
    DIR *src_dir = opendir(source_folder);
    if (!src_dir) {
        fprintf(stderr, "Error: opening source directory failed '%s': %s\n", source_folder, strerror(errno));
        return 1;
    }

    struct dirent *entry;
    // Перебираем все элементы в директории
    while ((entry = readdir(src_dir)) != NULL) {
        // Пропускаем текущую (.) и родительскую (..) директории
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Формируем полные пути к исходному и целевому элементам
        char src_path[512];
        char dst_path[512];
        snprintf(src_path, sizeof(src_path), "%s/%s", source_folder, entry->d_name);
        snprintf(dst_path, sizeof(dst_path), "%s/%s", dest_folder, entry->d_name);

        // Получаем информацию об исходном элементе
        struct stat src_stat;
        if (stat(src_path, &src_stat) != 0) {
            fprintf(stderr, "Warning: access to '%s' failed: %s\n", src_path, strerror(errno));
            continue;
        }

        // Обрабатываем поддиректорию рекурсивно
        if (S_ISDIR(src_stat.st_mode)) {
            BackupFolder(src_path, dst_path);
        }
        // Обрабатываем обычный файл
        else if (S_ISREG(src_stat.st_mode)) {
            // Формируем путь к сжатому файлу backup
            char dst_gz_path[528]; // dst_path + ".gz"
            snprintf(dst_gz_path, sizeof(dst_gz_path), "%s.gz", dst_path);
            
            // Проверяем, нужен ли backup этого файла
            if (NeedBackup(src_path, dst_gz_path)) {
                BackupFile(src_path, dst_path);
            } else {
                printf("File %s stil actual\n", src_path);
            }
        }
    }

    closedir(src_dir);
    return 0;
}

int main(int argc, char **argv) {
    // Проверяем количество аргументов
    if (argc != 3) {
        fprintf(stderr, "Error: There are should be 3 arguments!\n");
        fprintf(stderr, "Using: %s <source_direcctory> <target_direcctory>\n", argv[0]);
        return 1;
    }

    char *source_dir = argv[1];
    char *backup_dir = argv[2];

    // Проверяем существование исходной директории
    struct stat st;
    if (stat(source_dir, &st) != 0) {
        fprintf(stderr, "Error: source directory '%s' does not exists %s\n", source_dir, strerror(errno));
        return 1;
    }
    
    if (!S_ISDIR(st.st_mode)) {
        fprintf(stderr, "Error: '%s' is not directory\n", source_dir);
        return 1;
    }

    printf("Launching backup from '%s' to '%s'\n", source_dir, backup_dir);
    printf("========================================\n");
    
    int result = BackupFolder(source_dir, backup_dir);
    
    printf("========================================\n");
    if (result == 0) {
        printf("Backup successfuly completed\n");
    } else {
        printf("Backup completed with error\n");
    }
    
    return result;
}