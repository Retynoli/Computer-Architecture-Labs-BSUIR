# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.15

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
CMAKE_COMMAND = "/home/oem/Program Files/clion-2019.3.5/bin/cmake/linux/bin/cmake"

# The command to remove a file.
RM = "/home/oem/Program Files/clion-2019.3.5/bin/cmake/linux/bin/cmake" -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/oem/Documents/ACS/acs-spring-l3

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/oem/Documents/ACS/acs-spring-l3/cmake-build-debug

# Include any dependencies generated for this target.
include units/lib/googletest/CMakeFiles/gtest_main.dir/depend.make

# Include the progress variables for this target.
include units/lib/googletest/CMakeFiles/gtest_main.dir/progress.make

# Include the compile flags for this target's objects.
include units/lib/googletest/CMakeFiles/gtest_main.dir/flags.make

units/lib/googletest/CMakeFiles/gtest_main.dir/src/gtest_main.cc.o: units/lib/googletest/CMakeFiles/gtest_main.dir/flags.make
units/lib/googletest/CMakeFiles/gtest_main.dir/src/gtest_main.cc.o: ../units/lib/googletest/src/gtest_main.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/oem/Documents/ACS/acs-spring-l3/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object units/lib/googletest/CMakeFiles/gtest_main.dir/src/gtest_main.cc.o"
	cd /home/oem/Documents/ACS/acs-spring-l3/cmake-build-debug/units/lib/googletest && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/gtest_main.dir/src/gtest_main.cc.o -c /home/oem/Documents/ACS/acs-spring-l3/units/lib/googletest/src/gtest_main.cc

units/lib/googletest/CMakeFiles/gtest_main.dir/src/gtest_main.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/gtest_main.dir/src/gtest_main.cc.i"
	cd /home/oem/Documents/ACS/acs-spring-l3/cmake-build-debug/units/lib/googletest && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/oem/Documents/ACS/acs-spring-l3/units/lib/googletest/src/gtest_main.cc > CMakeFiles/gtest_main.dir/src/gtest_main.cc.i

units/lib/googletest/CMakeFiles/gtest_main.dir/src/gtest_main.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/gtest_main.dir/src/gtest_main.cc.s"
	cd /home/oem/Documents/ACS/acs-spring-l3/cmake-build-debug/units/lib/googletest && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/oem/Documents/ACS/acs-spring-l3/units/lib/googletest/src/gtest_main.cc -o CMakeFiles/gtest_main.dir/src/gtest_main.cc.s

# Object files for target gtest_main
gtest_main_OBJECTS = \
"CMakeFiles/gtest_main.dir/src/gtest_main.cc.o"

# External object files for target gtest_main
gtest_main_EXTERNAL_OBJECTS =

lib/libgtest_maind.a: units/lib/googletest/CMakeFiles/gtest_main.dir/src/gtest_main.cc.o
lib/libgtest_maind.a: units/lib/googletest/CMakeFiles/gtest_main.dir/build.make
lib/libgtest_maind.a: units/lib/googletest/CMakeFiles/gtest_main.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/oem/Documents/ACS/acs-spring-l3/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library ../../../lib/libgtest_maind.a"
	cd /home/oem/Documents/ACS/acs-spring-l3/cmake-build-debug/units/lib/googletest && $(CMAKE_COMMAND) -P CMakeFiles/gtest_main.dir/cmake_clean_target.cmake
	cd /home/oem/Documents/ACS/acs-spring-l3/cmake-build-debug/units/lib/googletest && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/gtest_main.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
units/lib/googletest/CMakeFiles/gtest_main.dir/build: lib/libgtest_maind.a

.PHONY : units/lib/googletest/CMakeFiles/gtest_main.dir/build

units/lib/googletest/CMakeFiles/gtest_main.dir/clean:
	cd /home/oem/Documents/ACS/acs-spring-l3/cmake-build-debug/units/lib/googletest && $(CMAKE_COMMAND) -P CMakeFiles/gtest_main.dir/cmake_clean.cmake
.PHONY : units/lib/googletest/CMakeFiles/gtest_main.dir/clean

units/lib/googletest/CMakeFiles/gtest_main.dir/depend:
	cd /home/oem/Documents/ACS/acs-spring-l3/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/oem/Documents/ACS/acs-spring-l3 /home/oem/Documents/ACS/acs-spring-l3/units/lib/googletest /home/oem/Documents/ACS/acs-spring-l3/cmake-build-debug /home/oem/Documents/ACS/acs-spring-l3/cmake-build-debug/units/lib/googletest /home/oem/Documents/ACS/acs-spring-l3/cmake-build-debug/units/lib/googletest/CMakeFiles/gtest_main.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : units/lib/googletest/CMakeFiles/gtest_main.dir/depend

