# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/elizabet/Repozitors/OC/Laba4

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/elizabet/Repozitors/OC/Laba4/build

# Include any dependencies generated for this target.
include CMakeFiles/power2_allocator.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/power2_allocator.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/power2_allocator.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/power2_allocator.dir/flags.make

CMakeFiles/power2_allocator.dir/memory_allocator.c.o: CMakeFiles/power2_allocator.dir/flags.make
CMakeFiles/power2_allocator.dir/memory_allocator.c.o: /home/elizabet/Repozitors/OC/Laba4/memory_allocator.c
CMakeFiles/power2_allocator.dir/memory_allocator.c.o: CMakeFiles/power2_allocator.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/elizabet/Repozitors/OC/Laba4/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/power2_allocator.dir/memory_allocator.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/power2_allocator.dir/memory_allocator.c.o -MF CMakeFiles/power2_allocator.dir/memory_allocator.c.o.d -o CMakeFiles/power2_allocator.dir/memory_allocator.c.o -c /home/elizabet/Repozitors/OC/Laba4/memory_allocator.c

CMakeFiles/power2_allocator.dir/memory_allocator.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/power2_allocator.dir/memory_allocator.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/elizabet/Repozitors/OC/Laba4/memory_allocator.c > CMakeFiles/power2_allocator.dir/memory_allocator.c.i

CMakeFiles/power2_allocator.dir/memory_allocator.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/power2_allocator.dir/memory_allocator.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/elizabet/Repozitors/OC/Laba4/memory_allocator.c -o CMakeFiles/power2_allocator.dir/memory_allocator.c.s

# Object files for target power2_allocator
power2_allocator_OBJECTS = \
"CMakeFiles/power2_allocator.dir/memory_allocator.c.o"

# External object files for target power2_allocator
power2_allocator_EXTERNAL_OBJECTS =

libpower2_allocator.so: CMakeFiles/power2_allocator.dir/memory_allocator.c.o
libpower2_allocator.so: CMakeFiles/power2_allocator.dir/build.make
libpower2_allocator.so: CMakeFiles/power2_allocator.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/elizabet/Repozitors/OC/Laba4/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C shared library libpower2_allocator.so"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/power2_allocator.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/power2_allocator.dir/build: libpower2_allocator.so
.PHONY : CMakeFiles/power2_allocator.dir/build

CMakeFiles/power2_allocator.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/power2_allocator.dir/cmake_clean.cmake
.PHONY : CMakeFiles/power2_allocator.dir/clean

CMakeFiles/power2_allocator.dir/depend:
	cd /home/elizabet/Repozitors/OC/Laba4/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/elizabet/Repozitors/OC/Laba4 /home/elizabet/Repozitors/OC/Laba4 /home/elizabet/Repozitors/OC/Laba4/build /home/elizabet/Repozitors/OC/Laba4/build /home/elizabet/Repozitors/OC/Laba4/build/CMakeFiles/power2_allocator.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/power2_allocator.dir/depend

