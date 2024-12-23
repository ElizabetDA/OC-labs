#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

typedef void *(*allocator_create_func)(size_t);
typedef void (*allocator_destroy_func)(void *);
typedef void *(*allocator_alloc_func)(void *, size_t);
typedef void (*allocator_free_func)(void *, void *);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <path_to_library>\n", argv[0]);
        return 1;
    }
    const char *library_path = argv[1];
    // Загрузка динамической библиотеки
    void *handle = dlopen(library_path, RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "Error loading library: %s\n", dlerror());
        return 1;
    }

    // Загрузка функций
    allocator_create_func allocator_create = (allocator_create_func)dlsym(handle, "allocator_create");
    allocator_destroy_func allocator_destroy = (allocator_destroy_func)dlsym(handle, "allocator_destroy");
    allocator_alloc_func allocator_alloc = (allocator_alloc_func)dlsym(handle, "allocator_alloc");
    allocator_free_func allocator_free = (allocator_free_func)dlsym(handle, "allocator_free");

    char *error = dlerror();
    if (error != NULL) {
        fprintf(stderr, "Error loading symbols: %s\n", error);
        dlclose(handle);
        return 1;
    }

    // Тестирование аллокатора
    size_t memory_size = 4096;
    void *allocator = allocator_create(memory_size);
    if (!allocator) {
        fprintf(stderr, "Failed to create allocator\n");
        dlclose(handle);
        return 1;
    }

    printf("Allocator created successfully.\n");

    void *block = allocator_alloc(allocator, 256);
    if (block) {
        printf("Memory block allocated successfully.\n");
    } else {
        fprintf(stderr, "Failed to allocate memory block.\n");
    }

    allocator_free(allocator, block);
    printf("Memory block freed successfully.\n");

    allocator_destroy(allocator);
    printf("Allocator destroyed successfully.\n");

    // Закрытие библиотеки
    dlclose(handle);
    return 0;
}
