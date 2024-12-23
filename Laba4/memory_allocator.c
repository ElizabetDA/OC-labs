#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <dlfcn.h>

//для аварии
void *fallback_allocator_create(size_t size) {
    void *memory = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (memory == MAP_FAILED) {
        const char *error_msg = "Fallback allocator: mmap failed\n";
        write(STDERR_FILENO, error_msg, strlen(error_msg));
    }
    return memory;
}

void fallback_allocator_destroy(void *memory, size_t size) {
    if (memory != NULL && munmap(memory, size) == -1) {
        const char *error_msg = "Fallback allocator: munmap failed\n";
        write(STDERR_FILENO, error_msg, strlen(error_msg));
    }
}

void *fallback_allocator_alloc(void *memory, size_t size, size_t total_size) {
    static size_t offset = 0;
    if (offset + size > total_size) {
        const char *error_msg = "Fallback allocator: not enough memory\n";
        write(STDERR_FILENO, error_msg, strlen(error_msg));
        return NULL;
    }
    void *allocated = (void *)((char *)memory + offset);
    offset += size;
    return allocated;
}

void fallback_allocator_free(void *ptr) {
    // fallback не управляет памятью динамически.
}

//списки свободных блоков
typedef struct Block {
    size_t size;
    struct Block *next;
    bool is_free;
} Block;

typedef struct Allocator {
    Block *free_list;
    void *memory;
    size_t size;
} Allocator;

Allocator *allocator_create(size_t size) {
    if (size < sizeof(Allocator) + sizeof(Block)) {
        const char *error_msg = "Allocator: size too small for creation\n";
        write(STDERR_FILENO, error_msg, strlen(error_msg));
        return NULL;
    }

    void *memory = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (memory == MAP_FAILED) {
        const char *error_msg = "Allocator: mmap failed\n";
        write(STDERR_FILENO, error_msg, strlen(error_msg));
        return (Allocator *)fallback_allocator_create(size);
    }

    Allocator *allocator = (Allocator *)memory;
    allocator->memory = (void *)((char *)memory + sizeof(Allocator));
    allocator->size = size - sizeof(Allocator);
    allocator->free_list = (Block *)allocator->memory;

    allocator->free_list->size = allocator->size - sizeof(Block);
    allocator->free_list->next = NULL;
    allocator->free_list->is_free = true;

    return allocator;
}

void *allocator_alloc(Allocator *allocator, size_t size) {
    if (allocator == NULL || size == 0) {
        const char *error_msg = "Allocator: invalid allocator or size\n";
        write(STDERR_FILENO, error_msg, strlen(error_msg));
        return NULL;
    }

    Block *current = allocator->free_list;
    while (current != NULL) {
        if (current->is_free && current->size >= size) {
            if (current->size > size + sizeof(Block)) {
                Block *new_block = (Block *)((char *)current + sizeof(Block) + size);
                new_block->size = current->size - size - sizeof(Block);
                new_block->next = current->next;
                new_block->is_free = true;

                current->size = size;
                current->next = new_block;
            }

            current->is_free = false;
            return (void *)((char *)current + sizeof(Block));
        }
        current = current->next;
    }

    const char *error_msg = "Allocator: no suitable block found\n";
    write(STDERR_FILENO, error_msg, strlen(error_msg));
    return NULL;
}

void allocator_free(Allocator *allocator, void *ptr) {
    if (allocator == NULL || ptr == NULL) {
        const char *error_msg = "Allocator: invalid free operation\n";
        write(STDERR_FILENO, error_msg, strlen(error_msg));
        return;
    }

    Block *block = (Block *)((char *)ptr - sizeof(Block));
    block->is_free = true;

    Block *current = allocator->free_list;
    while (current != NULL && current->next != NULL) {
        if (current->is_free && current->next->is_free) {
            current->size += sizeof(Block) + current->next->size;
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
}

void allocator_destroy(Allocator *allocator) {
    if (allocator == NULL) {
        const char *error_msg = "Allocator: invalid destroy operation\n";
        write(STDERR_FILENO, error_msg, strlen(error_msg));
        return;
    }
    if (munmap(allocator, allocator->size + sizeof(Allocator)) == -1) {
        const char *error_msg = "Allocator: munmap failed\n";
        write(STDERR_FILENO, error_msg, strlen(error_msg));
    }
}

//блоки по 2^n
#define PAGE_SIZE 4096

typedef struct Power2Block {
    size_t block_size;
    struct Power2Block *next_free;
    bool is_free;
} Power2Block;

typedef struct Page {
    size_t page_size;
    struct Page *next_free;
    Power2Block *free_blocks;
} Page;

typedef struct Power2Allocator {
    void *memory;
    size_t size;
    Page *free_pages;
} Power2Allocator;

Power2Allocator *allocator_create_power2(size_t size) {
    if (size < PAGE_SIZE) {
        const char *error_msg = "Power2 Allocator: size too small for creation\n";
        write(STDERR_FILENO, error_msg, strlen(error_msg));
        return NULL;
    }
    void *memory = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (memory == MAP_FAILED) {
        const char *error_msg = "Power2 Allocator: mmap failed\n";
        write(STDERR_FILENO, error_msg, strlen(error_msg));
        return (Power2Allocator *)fallback_allocator_create(size);
    }
    Power2Allocator *allocator = (Power2Allocator *)memory;
    allocator->memory = (uint8_t *)memory + sizeof(Power2Allocator);
    allocator->size = size - sizeof(Power2Allocator);
    allocator->free_pages = (Page *)allocator->memory;

    size_t num_pages = allocator->size / PAGE_SIZE;
    for (size_t i = 0; i < num_pages; i++) {
        Page *page = (Page *)((uint8_t *)allocator->memory + i * PAGE_SIZE);
        page->page_size = PAGE_SIZE;
        page->next_free = (i == num_pages - 1) ? NULL : (Page *)((uint8_t *)allocator->memory + (i + 1) * PAGE_SIZE);
        page->free_blocks = NULL;
    }
    return allocator;
}

void *allocator_alloc_power2(Power2Allocator *allocator, const size_t size) {
    if (!allocator || size == 0) {
        const char *error_msg = "Power2 Allocator: invalid alloc operation\n";
        write(STDERR_FILENO, error_msg, strlen(error_msg));
        return NULL;
    }

    size_t block_size = 1;
    while (block_size < size + sizeof(Power2Block) && block_size < PAGE_SIZE) {
        block_size *= 2;
    }

    Page *page = allocator->free_pages;
    while (page != NULL) {
        Power2Block *block = page->free_blocks;
        Power2Block *prev = NULL;

        while (block != NULL) {
            if (block->block_size >= block_size && block->is_free) {
                if (prev == NULL) {
                    page->free_blocks = block->next_free;
                } else {
                    prev->next_free = block->next_free;
                }
                block->is_free = false;
                return (void *)((uint8_t *)block + sizeof(Power2Block));
            }
            prev = block;
            block = block->next_free;
        }
        page = page->next_free;
    }

    const char *error_msg = "Power2 Allocator: no suitable block found\n";
    write(STDERR_FILENO, error_msg, strlen(error_msg));
    return NULL;
}

void allocator_free_power2(Power2Allocator *allocator, void *ptr) {
    if (!allocator || !ptr) {
        const char *error_msg = "Power2 Allocator: invalid free operation\n";
        write(STDERR_FILENO, error_msg, strlen(error_msg));
        return;
    }

    Power2Block *block = (Power2Block *)((uint8_t *)ptr - sizeof(Power2Block));

    Page *page = allocator->free_pages;
    while (page != NULL) {
        if ((uint8_t *)block >= (uint8_t *)page && (uint8_t *)block < (uint8_t *)page + PAGE_SIZE) {
            block->next_free = page->free_blocks;
            block->is_free = true;
            page->free_blocks = block;
            return;
        }
        page = page->next_free;
    }
}

void allocator_destroy_power2(Power2Allocator *allocator) {
    if (!allocator) {
        const char *error_msg = "Power2 Allocator: invalid destroy operation\n";
        write(STDERR_FILENO, error_msg, strlen(error_msg));
        return;
    }
    if (munmap(allocator, allocator->size + sizeof(Power2Allocator)) == -1) {
        const char *error_msg = "Power2 Allocator: munmap failed\n";
        write(STDERR_FILENO, error_msg, strlen(error_msg));
    }
}