#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>

//динамическая загрузка
typedef void *(*allocator_create_func)(size_t size);
typedef void (*allocator_destroy_func)(void *allocator);
typedef void *(*allocator_alloc_func)(void *allocator, size_t size);
typedef void (*allocator_free_func)(void *allocator, void *ptr);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        const char *error_msg = "Usage: ./program <path_to_library>\n";
        write(STDERR_FILENO, error_msg, strlen(error_msg));
        return 1;
    }
    const char *library_path = argv[1];
    // для библиотеки
    void *handle = dlopen(library_path, RTLD_LAZY);
    if (!handle) {
        write(STDERR_FILENO, dlerror(), strlen(dlerror()));
        return 1;
    }

    // для функций из библиотеки
    allocator_create_func allocator_create = (allocator_create_func)dlsym(handle, "allocator_create");
    allocator_destroy_func allocator_destroy = (allocator_destroy_func)dlsym(handle, "allocator_destroy");
    allocator_alloc_func allocator_alloc = (allocator_alloc_func)dlsym(handle, "allocator_alloc");
    allocator_free_func allocator_free = (allocator_free_func)dlsym(handle, "allocator_free");

    char *error;
    if ((error = dlerror()) != NULL) {
        write(STDERR_FILENO, error, strlen(error));
        dlclose(handle);
        return 1;
    }

    // используем функции аллокатора
    size_t memory_size = 4096;
    void *allocator = allocator_create(memory_size);
    if (!allocator) {
        const char *error_msg = "Failed to create allocator\n";
        write(STDERR_FILENO, error_msg, strlen(error_msg));
        dlclose(handle);
        return 1;
    }

    void *block = allocator_alloc(allocator, 256);
    if (!block) {
        const char *error_msg = "Failed to allocate memory\n";
        write(STDERR_FILENO, error_msg, strlen(error_msg));
    } else {
        const char *success_msg = "Memory allocated successfully\n";
        write(STDOUT_FILENO, success_msg, strlen(success_msg));
    }

    allocator_free(allocator, block);
    allocator_destroy(allocator);

    // закрываем библиотеку
    dlclose(handle);

    return 0;
}