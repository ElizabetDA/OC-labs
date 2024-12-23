#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define FILE_SIZE 4096
#define BUFFER_SIZE 256

volatile sig_atomic_t parent_signaled = 0;

void handle_signal(int signo) {
    if (signo == SIGUSR1) {
        parent_signaled = 1;
    }
}

void remove_vowels(const char *input, char *output) {
    const char *vowels = "aeiouAEIOU";
    int j = 0;
    for (int i = 0; input[i] != '\0'; i++) {
        if (strchr(vowels, input[i]) == NULL) {
            output[j++] = input[i];
        }
    }
    output[j] = '\0';
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        const char *error = "Usage: child <file_name>\n";
        write(STDERR_FILENO, error, strlen(error));
        exit(EXIT_FAILURE);
    }

    const char *file_name = argv[1];

    int fd = open(file_name, O_RDWR);
    if (fd == -1) {
        write(STDERR_FILENO, "Failed to open file\n", 20);
        exit(EXIT_FAILURE);
    }

    char *mapped_memory = mmap(NULL, FILE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mapped_memory == MAP_FAILED) {
        write(STDERR_FILENO, "Failed to mmap file\n", 21);
        close(fd);
        exit(EXIT_FAILURE);
    }

    close(fd);

    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        write(STDERR_FILENO, "Failed to set signal handler\n", 30);
        munmap(mapped_memory, FILE_SIZE);
        exit(EXIT_FAILURE);
    }

    char result[BUFFER_SIZE];

    while (1) {
        while (!parent_signaled);
        parent_signaled = 0;

        remove_vowels(mapped_memory, result);

        strncpy(mapped_memory, result, FILE_SIZE - 1);

        kill(getppid(), SIGUSR1);
    }

    munmap(mapped_memory, FILE_SIZE);
    return 0;
}
