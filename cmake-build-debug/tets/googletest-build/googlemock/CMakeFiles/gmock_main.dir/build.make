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
include tets/googletest-build/googlemock/CMakeFiles/gmock_main.dir/depend.make

# Include the progress variables for this target.
include tets/googletest-build/googlemock/CMakeFiles/gmock_main.dir/progress.make

# Include the compile flags for this target's objects.
include tets/googletest-build/googlemock/CMakeFiles/gmock_main.dir/flags.make

tets/googletest-build/googlemock/CMakeFiles/gmock_main.dir/src/gmock_main.cc.o: tets/googletest-build/googlemock/CMakeFiles/gmock_main.dir/flags.make
tets/googletest-build/googlemock/CMakeFiles/gmock_main.dir/src/gmock_main.cc.o: tets/googletest-src/googlemock/src/gmock_main.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/cs/usr/doronbruder/CLionProjects/final1/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object tets/googletest-build/googlemock/CMakeFiles/gmock_main.dir/src/gmock_main.cc.o"
	cd /cs/usr/doronbruder/CLionProjects/final1/cmake-build-debug/tets/googletest-build/googlemock && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/gmock_main.dir/src/gmock_main.cc.o -c /cs/usr/doronbruder/CLionProjects/final1/cmake-build-debug/tets/googletest-src/googlemock/src/gmock_main.cc

tets/googletest-build/googlemock/CMakeFiles/gmock_main.dir/src/gmock_main.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/gmock_main.dir/src/gmock_main.cc.i"
	cd /cs/usr/doronbruder/CLionProjects/final1/cmake-build-debug/tets/googletest-build/googlemock && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /cs/usr/doronbruder/CLionProjects/final1/cmake-build-debug/tets/googletest-src/googlemock/src/gmock_main.cc > CMakeFiles/gmock_main.dir/src/gmock_main.cc.i

tets/googletest-build/googlemock/CMakeFiles/gmock_main.dir/src/gmock_main.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/gmock_main.dir/src/gmock_main.cc.s"
	cd /cs/usr/doronbruder/CLionProjects/final1/cmake-build-debug/tets/googletest-build/googlemock && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /cs/usr/doronbruder/CLionProjects/final1/cmake-build-debug/tets/googletest-src/googlemock/src/gmock_main.cc -o CMakeFiles/gmock_main.dir/src/gmock_main.cc.s

# Object files for target gmock_main
gmock_main_OBJECTS = \
"CMakeFiles/gmock_main.dir/src/gmock_main.cc.o"

# External object files for target gmock_main
gmock_main_EXTERNAL_OBJECTS =

lib/libgmock_maind.a: tets/googletest-build/googlemock/CMakeFiles/gmock_main.dir/src/gmock_main.cc.o
lib/libgmock_maind.a: tets/googletest-build/googlemock/CMakeFiles/gmock_main.dir/build.make
lib/libgmock_maind.a: tets/googletest-build/googlemock/CMakeFiles/gmock_main.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/cs/usr/doronbruder/CLionProjects/final1/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library ../../../lib/libgmock_maind.a"
	cd /cs/usr/doronbruder/CLionProjects/final1/cmake-build-debug/tets/googletest-build/googlemock && $(CMAKE_COMMAND) -P CMakeFiles/gmock_main.dir/cmake_clean_target.cmake
	cd /cs/usr/doronbruder/CLionProjects/final1/cmake-build-debug/tets/googletest-build/googlemock && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/gmock_main.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tets/googletest-build/googlemock/CMakeFiles/gmock_main.dir/build: lib/libgmock_maind.a

.PHONY : tets/googletest-build/googlemock/CMakeFiles/gmock_main.dir/build

tets/googletest-build/googlemock/CMakeFiles/gmock_main.dir/clean:
	cd /cs/usr/doronbruder/CLionProjects/final1/cmake-build-debug/tets/googletest-build/googlemock && $(CMAKE_COMMAND) -P CMakeFiles/gmock_main.dir/cmake_clean.cmake
.PHONY : tets/googletest-build/googlemock/CMakeFiles/gmock_main.dir/clean

tets/googletest-build/googlemock/CMakeFiles/gmock_main.dir/depend:
	cd /cs/usr/doronbruder/CLionProjects/final1/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /cs/usr/doronbruder/CLionProjects/final1 /cs/usr/doronbruder/CLionProjects/final1/cmake-build-debug/tets/googletest-src/googlemock /cs/usr/doronbruder/CLionProjects/final1/cmake-build-debug /cs/usr/doronbruder/CLionProjects/final1/cmake-build-debug/tets/googletest-build/googlemock /cs/usr/doronbruder/CLionProjects/final1/cmake-build-debug/tets/googletest-build/googlemock/CMakeFiles/gmock_main.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tets/googletest-build/googlemock/CMakeFiles/gmock_main.dir/depend
