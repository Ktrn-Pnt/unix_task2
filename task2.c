#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#define ERROR_COUNT_ARGS "ERROR: PID = %d, Invalid count of args, enter file to lock\n"
#define ERROR_WRITE_TO_LOCK_FILE "ERROR: PID = %d, Can't write to .lck file\n"
#define ERROR_MODIFY_FILE "ERROR: PID = %d, Can't modify file\n"
#define ERROR_UNLOCK_FILE "ERROR: PID = %d, Can't unlock file\n"
#define LOCK_SUF ".lck"
#define PERMISSION 0664
#define LOCK_TIME 1

pid_t pid;
int locks = 0;

void sigint_handler(int sig) {
    printf("PID = %d, success locks: %d\n", pid, locks);
    exit(0);
}

int lock(char* file_name_lock) {

    int fd_lock = -1;
    while (fd_lock == -1) {
        fd_lock = open(file_name_lock, O_CREAT | O_EXCL | O_WRONLY, PERMISSION);
    }

    char buffer[10];
    sprintf(buffer, "%d", pid);
    int pid_len = strlen(buffer);

    int write_result = write(fd_lock, buffer, pid_len);
    close(fd_lock);

    return write_result;
}

int unlock(char* file_name_lock) {

    int lock_fd = open(file_name_lock, O_RDONLY, PERMISSION);

    if (lock_fd == -1) {
        return -1;
    }

    char data_from_lock[10];

    int read_result = read(lock_fd, data_from_lock, 10);
    if (read_result == -1) {
        return -1;
    }

    int pid_from_lock = atoi(data_from_lock);

    close(lock_fd);

    if (pid_from_lock != pid) {
        return -1;
    }

    int remove_result = remove(file_name_lock);
    return remove_result;
}

int change_file_with_sleep(char* file_name) {
    
    int file_fd = open(file_name, O_RDWR | O_APPEND, PERMISSION);

    if (file_fd == -1) {
        return -1;
    }

    char buffer_file[1];

    int read_result = read(file_fd, buffer_file, 1);
    if (read_result == -1) {
        return -1;
    }

    int write_result = write(file_fd, buffer_file, 1);
    if (write_result == -1) {
        return -1;
    }

    close(file_fd);

    sleep(LOCK_TIME);

    return 0;
}


int main(int argc, char* argv[]) {

    signal(SIGINT, sigint_handler);

    pid = getpid();

    if (argc < 2) {
        printf(ERROR_COUNT_ARGS, pid);
        return 1;
    }

    char* file_name = argv[1];

    char* file_name_lock = malloc(sizeof(file_name) + strlen(LOCK_SUF));
    sprintf(file_name_lock,"%s%s", file_name, LOCK_SUF);

    while (1) {
        int lock_result = lock(file_name_lock);

        if (lock_result == -1) {
            printf(ERROR_WRITE_TO_LOCK_FILE, pid);
            return 1;
        }

        int change_file_result = change_file_with_sleep(file_name);

        if (change_file_result == -1) {
            printf(ERROR_MODIFY_FILE, pid);
            return 1;
        }

        int unlock_result = unlock(file_name_lock);

        if (unlock_result == -1) {
            printf(ERROR_UNLOCK_FILE, pid);
            return 1;
        }

        locks++;
    }

    return 0; 
}