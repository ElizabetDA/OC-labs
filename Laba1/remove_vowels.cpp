#include <unistd.h>
#include <fcntl.h>
#include <cstring>

bool isVowel(char c) {
    char vowels[] = "AEIOUaeiou";
    for (int i = 0; i < 10; ++i) {
        if (c == vowels[i]) {
            return true;
        }
    }
    return false;
}

int main() {
    char buffer[1024];
    ssize_t bytesRead = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);

    if (bytesRead < 0) {
        const char* errorMsg = "Read failed\n";
        write(STDERR_FILENO, errorMsg, strlen(errorMsg));
        _exit(1);
    }

    buffer[bytesRead] = '\0';

    // Проходим по каждому символу и выводим только согласные
    for (int i = 0; i < bytesRead; ++i) {
        if (!isVowel(buffer[i])) {
            write(STDOUT_FILENO, &buffer[i], 1);
        }
    }

    return 0;
}
