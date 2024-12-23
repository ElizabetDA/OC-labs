#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <cstring>
#include <cstdlib>

int main() {
    int pipe1[2], pipe2[2];

    // Создание двух пайпов
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        const char* errorMsg = "Pipe creation failed\n";
        write(STDERR_FILENO, errorMsg, strlen(errorMsg));
        exit(1);
    }

    // Создание первого дочернего процесса
    pid_t child1 = fork();
    if (child1 < 0) {
        const char* errorMsg = "Fork failed for child1\n";
        write(STDERR_FILENO, errorMsg, strlen(errorMsg));
        exit(1);
    }

    if (child1 == 0) {
        // Во первом дочернем процессе
        close(pipe1[1]); // Закрываем конец для записи
        dup2(pipe1[0], STDIN_FILENO); // Перенаправляем стандартный ввод на pipe1
        close(pipe1[0]); // Закрываем конец для чтения
        execl("./remove_vowels", "remove_vowels", (char *)NULL); // Запуск нового исполняемого файла
        perror("execl failed"); // Если execl не сработает
        _exit(1);
    }

    // Создание второго дочернего процесса
    pid_t child2 = fork();
    if (child2 < 0) {
        const char* errorMsg = "Fork failed for child2\n";
        write(STDERR_FILENO, errorMsg, strlen(errorMsg));
        exit(1);
    }

    if (child2 == 0) {
        // Во втором дочернем процессе
        close(pipe2[1]); // Закрываем конец для записи
        dup2(pipe2[0], STDIN_FILENO); // Перенаправляем стандартный ввод на pipe2
        close(pipe2[0]); // Закрываем конец для чтения
        execl("./remove_vowels", "remove_vowels", (char *)NULL); // Запуск нового исполняемого файла
        perror("execl failed"); // Если execl не сработает
        _exit(1);
    }

    // В родительском процессе
    close(pipe1[0]); // Закрываем конец для чтения pipe1
    close(pipe2[0]); // Закрываем конец для чтения pipe2

    // Чтение строки от пользователя
    char input[1024];
    ssize_t len = read(STDIN_FILENO, input, sizeof(input) - 1);
    if (len < 0) {
        const char* errorMsg = "Input failed\n";
        write(STDERR_FILENO, errorMsg, strlen(errorMsg));
        exit(1);
    }

    input[len] = '\0';
    // С вероятностью 80% отправляем строку в pipe1, иначе в pipe2
    if (rand() % 10 < 8) {
        write(pipe1[1], input, len);
    } else {
        write(pipe2[1], input, len);  //делать проверки
    }

    close(pipe1[1]); // Закрываем конец для записи pipe1
    close(pipe2[1]); // Закрываем конец для записи pipe2

    // Ожидание завершения дочерних процессов
    wait(NULL);
    wait(NULL);

    return 0;
}


/*STDIN_FILENO это файловый дескриптор (файл, сокет, пайп и т. д.; тип int). Используется не буферизованный ввод/вывод, а прямой вызов системных функций*/