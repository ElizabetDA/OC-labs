#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
//padg hit foult
#define FILE_SIZE 4096
#define FILE_NAME "shared_memory_file"
#define BUFFER_SIZE 256

volatile sig_atomic_t child_ready = 0;

void handle_signal(int signo) {
    if (signo == SIGUSR1) {
        child_ready = 1;
    }
}

void safe_write(int fd, const char *buffer) {
    if (write(fd, buffer, strlen(buffer)) == -1) {
        _exit(EXIT_FAILURE);
    }
}

void safe_read(int fd, char *buffer, size_t size) {
    ssize_t len = read(fd, buffer, size);
    if (len == -1) {
        const char *error = "Ошибка чтения из stdin\n";
        write(STDERR_FILENO, error, strlen(error));
        _exit(EXIT_FAILURE);
    }
    buffer[len] = '\0';
}

int main() {
    int fd = open(FILE_NAME, O_CREAT | O_RDWR, 0644);
    if (fd == -1) {
        write(STDERR_FILENO, "Failed to open file\n", 20);
        exit(EXIT_FAILURE);
    }

    if (ftruncate(fd, FILE_SIZE) == -1) {
        write(STDERR_FILENO, "Failed to set file size\n", 24);
        close(fd);
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

    pid_t pid = fork();
    if (pid == -1) {
        write(STDERR_FILENO, "Failed to fork\n", 16);
        munmap(mapped_memory, FILE_SIZE);
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        execl("./child", "./child", FILE_NAME, NULL);
        write(STDERR_FILENO, "Failed to execute child\n", 24);
        _exit(EXIT_FAILURE);
    }

    const char *prompt = "Введите строки (для завершения введите 'exit'):\n";
    safe_write(STDOUT_FILENO, prompt);

    char input[BUFFER_SIZE];
    while (1) {
        safe_read(STDIN_FILENO, input, BUFFER_SIZE - 1);
        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "exit") == 0) {
            break; // Не обрабатываем "exit", просто выходим
        }

        // Очищаем память перед записью
        memset(mapped_memory, 0, FILE_SIZE);

        // Пишем строку в отображаемую память
        strncpy(mapped_memory, input, FILE_SIZE - 1);

        // Отправляем сигнал дочернему процессу
        kill(pid, SIGUSR1);

        // Ждём ответа от дочернего процесса
        while (!child_ready);
        child_ready = 0;

        const char *result_msg = "Результат: ";
        safe_write(STDOUT_FILENO, result_msg);
        safe_write(STDOUT_FILENO, mapped_memory);
        safe_write(STDOUT_FILENO, "\n");
    }

    // Завершаем работу
    kill(pid, SIGTERM);
    wait(NULL);

    munmap(mapped_memory, FILE_SIZE);
    unlink(FILE_NAME);

    const char *exit_msg = "Родительский процесс завершён.\n";
    safe_write(STDOUT_FILENO, exit_msg);

    return 0;
}
