# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.14

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/APP/jetbrains/clion/2019.2.1/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /usr/local/APP/jetbrains/clion/2019.2.1/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /cs/usr/doronbruder/CLionProjects/final1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /cs/usr/doronbruder/CLionProjects/final1/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/uthreads.h.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/uthreads.h.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/uthreads.h.dir/flags.make

CMakeFiles/uthreads.h.dir/uthreads.cpp.o: CMakeFiles/uthreads.h.dir/flags.make
CMakeFiles/uthreads.h.dir/uthreads.cpp.o: ../uthreads.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/cs/usr/doronbruder/CLionProjects/final1/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/uthreads.h.dir/uthreads.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/uthreads.h.dir/uthreads.cpp.o -c /cs/usr/doronbruder/CLionProjects/final1/uthreads.cpp

CMakeFiles/uthreads.h.dir/uthreads.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/uthreads.h.dir/uthreads.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /cs/usr/doronbruder/CLionProjects/final1/uthreads.cpp > CMakeFiles/uthreads.h.dir/uthreads.cpp.i

CMakeFiles/uthreads.h.dir/uthreads.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/uthreads.h.dir/uthreads.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /cs/usr/doronbruder/CLionProjects/final1/uthreads.cpp -o CMakeFiles/uthreads.h.dir/uthreads.cpp.s

CMakeFiles/uthreads.h.dir/Thread.cpp.o: CMakeFiles/uthreads.h.dir/flags.make
CMakeFiles/uthreads.h.dir/Thread.cpp.o: ../Thread.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/cs/usr/doronbruder/CLionProjects/final1/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/uthreads.h.dir/Thread.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/uthreads.h.dir/Thread.cpp.o -c /cs/usr/doronbruder/CLionProjects/final1/Thread.cpp

CMakeFiles/uthreads.h.dir/Thread.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/uthreads.h.dir/Thread.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /cs/usr/doronbruder/CLionProjects/final1/Thread.cpp > CMakeFiles/uthreads.h.dir/Thread.cpp.i

CMakeFiles/uthreads.h.dir/Thread.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/uthreads.h.dir/Thread.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /cs/usr/doronbruder/CLionProjects/final1/Thread.cpp -o CMakeFiles/uthreads.h.dir/Thread.cpp.s

# Object files for target uthreads.h
uthreads_h_OBJECTS = \
"CMakeFiles/uthreads.h.dir/uthreads.cpp.o" \
"CMakeFiles/uthreads.h.dir/Thread.cpp.o"

# External object files for target uthreads.h
uthreads_h_EXTERNAL_OBJECTS =

uthreads.h: CMakeFiles/uthreads.h.dir/uthreads.cpp.o
uthreads.h: CMakeFiles/uthreads.h.dir/Thread.cpp.o
uthreads.h: CMakeFiles/uthreads.h.dir/build.make
uthreads.h: CMakeFiles/uthreads.h.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/cs/usr/doronbruder/CLionProjects/final1/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable uthreads.h"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/uthreads.h.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/uthreads.h.dir/build: uthreads.h

.PHONY : CMakeFiles/uthreads.h.dir/build

CMakeFiles/uthreads.h.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/uthreads.h.dir/cmake_clean.cmake
.PHONY : CMakeFiles/uthreads.h.dir/clean

CMakeFiles/uthreads.h.dir/depend:
	cd /cs/usr/doronbruder/CLionProjects/final1/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /cs/usr/doronbruder/CLionProjects/final1 /cs/usr/doronbruder/CLionProjects/final1 /cs/usr/doronbruder/CLionProjects/final1/cmake-build-debug /cs/usr/doronbruder/CLionProjects/final1/cmake-build-debug /cs/usr/doronbruder/CLionProjects/final1/cmake-build-debug/CMakeFiles/uthreads.h.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/uthreads.h.dir/depend
