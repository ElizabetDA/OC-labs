# Минимальная версия CMake
cmake_minimum_required(VERSION 3.10)

# Название проекта
project(MemoryMappedProcesses LANGUAGES C)

# Устанавливаем стандарт языка C
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

# Указываем исходные файлы для parent и child
add_executable(parent parent.c)
add_executable(child child.c)

# Добавляем сообщения компилятора для родителя и ребенка
target_compile_options(parent PRIVATE -Wall -Wextra -Wpedantic)
target_compile_options(child PRIVATE -Wall -Wextra -Wpedantic)
