#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <stdio.h>

// Глобальные переменные для синхронизации
pthread_mutex_t min_max_mutex = PTHREAD_MUTEX_INITIALIZER;
int global_min = INT_MAX;
int global_max = INT_MIN;

typedef struct {
    int **matrix;
    int **result;
    int rows;         // Кол-во строк
    int cols;         // Кол-во столбцов
    int window_size;  // Размер окна медианного фильтра
    int start_row;
    int end_row;
} ThreadData;

int compare(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

int **allocate_matrix(int rows, int cols) {
    int **matrix = malloc(rows * sizeof(int *));
    if (!matrix) {
        write(STDERR_FILENO, "Ошибка выделения памяти для строк матрицы\n", 42);
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < rows; i++) {
        matrix[i] = malloc(cols * sizeof(int));
        if (!matrix[i]) {
            write(STDERR_FILENO, "Ошибка выделения памяти для столбцов матрицы\n", 46);
            exit(EXIT_FAILURE);
        }
    }
    return matrix;
}

void free_matrix(int **matrix, int rows) {
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

void apply_median_filter(ThreadData *data) {
    int w = data->window_size;
    int half_w = w / 2;
    int size = w * w;
    int *window = malloc(size * sizeof(int));
    if (!window) {
        write(STDERR_FILENO, "Ошибка выделения памяти для окна\n", 33);
        exit(EXIT_FAILURE);
    }

    for (int i = data->start_row; i < data->end_row; i++) {
        for (int j = 0; j < data->cols; j++) {
            int count = 0;
            for (int ki = -half_w; ki <= half_w; ki++) {
                for (int kj = -half_w; kj <= half_w; kj++) {
                    int ni = i + ki, nj = j + kj;
                    if (ni >= 0 && ni < data->rows && nj >= 0 && nj < data->cols) {
                        window[count++] = data->matrix[ni][nj];
                    }
                }
            }
            qsort(window, count, sizeof(int), compare);
            data->result[i][j] = window[count / 2];

            // Обновляем глобальные минимумы и максимумы
            int value = window[count / 2];
            pthread_mutex_lock(&min_max_mutex);
            if (value < global_min) global_min = value;
            if (value > global_max) global_max = value;
            pthread_mutex_unlock(&min_max_mutex);
        }
    }

    free(window);
}

void *thread_function(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    apply_median_filter(data);
    return NULL;
}

int str_to_int(const char *str) {
    char *endptr;
    int value = strtol(str, &endptr, 10);
    if (*endptr != '\0') {
        write(STDERR_FILENO, "Ошибка: некорректный ввод числа\n", 31);
        exit(EXIT_FAILURE);
    }
    return value;
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        write(STDERR_FILENO, "Использование: ./program <строки> <столбцы> <размер_окна> <потоки>\n", 68);
        return EXIT_FAILURE;
    }

    int rows = str_to_int(argv[1]);
    int cols = str_to_int(argv[2]);
    int window_size = str_to_int(argv[3]);
    int thread_count = str_to_int(argv[4]);

    if (rows <= 0 || cols <= 0 || window_size <= 0 || thread_count <= 0) {
        write(STDERR_FILENO, "Ошибка: недопустимые значения аргументов\n", 41);
        return EXIT_FAILURE;
    }

    int max_threads = sysconf(_SC_NPROCESSORS_ONLN);
    if (thread_count > max_threads) {
        thread_count = max_threads;
    }

    int **matrix = allocate_matrix(rows, cols);
    int **result = allocate_matrix(rows, cols);

    srand(time(NULL));
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            matrix[i][j] = rand() % 1000;
        }
    }

    char buffer[1024];
    int offset = 0;
    offset += snprintf(buffer + offset, sizeof(buffer) - offset, "Исходная матрица:\n");
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            offset += snprintf(buffer + offset, sizeof(buffer) - offset, "%3d ", matrix[i][j]);
        }
        offset += snprintf(buffer + offset, sizeof(buffer) - offset, "\n");
    }
    write(STDOUT_FILENO, buffer, offset);

    pthread_t threads[thread_count];
    ThreadData thread_data[thread_count];

    int rows_per_thread = rows / thread_count;

    for (int i = 0; i < thread_count; i++) {
        thread_data[i].matrix = matrix;
        thread_data[i].result = result;
        thread_data[i].rows = rows;
        thread_data[i].cols = cols;
        thread_data[i].window_size = window_size;
        thread_data[i].start_row = i * rows_per_thread;
        thread_data[i].end_row = (i == thread_count - 1) ? rows : (i + 1) * rows_per_thread;

        if (pthread_create(&threads[i], NULL, thread_function, &thread_data[i]) != 0) {
            write(STDERR_FILENO, "Ошибка создания потока\n", 23);
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < thread_count; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            write(STDERR_FILENO, "Ошибка завершения потока\n", 25);
            return EXIT_FAILURE;
        }
    }

    offset = 0;
    offset += snprintf(buffer + offset, sizeof(buffer) - offset, "Обработанная матрица:\n");
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            offset += snprintf(buffer + offset, sizeof(buffer) - offset, "%3d ", result[i][j]);
        }
        offset += snprintf(buffer + offset, sizeof(buffer) - offset, "\n");
    }
    write(STDOUT_FILENO, buffer, offset);

    // Вывод глобальных минимального и максимального значений
    offset = 0;
    offset += snprintf(buffer + offset, sizeof(buffer) - offset, "Глобальный минимум: %d\n", global_min);
    offset += snprintf(buffer + offset, sizeof(buffer) - offset, "Глобальный максимум: %d\n", global_max);
    write(STDOUT_FILENO, buffer, offset);

    free_matrix(matrix, rows);
    free_matrix(result, rows);

    pthread_mutex_destroy(&min_max_mutex);

    return EXIT_SUCCESS;
}
