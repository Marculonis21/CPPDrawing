# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/marculonis/Desktop/Projects/CPP/CPPDrawing/compute_test

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/marculonis/Desktop/Projects/CPP/CPPDrawing/compute_test/build

# Include any dependencies generated for this target.
include CMakeFiles/computeTest.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/computeTest.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/computeTest.dir/flags.make

CMakeFiles/computeTest.dir/src/main.cpp.o: CMakeFiles/computeTest.dir/flags.make
CMakeFiles/computeTest.dir/src/main.cpp.o: ../src/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/marculonis/Desktop/Projects/CPP/CPPDrawing/compute_test/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/computeTest.dir/src/main.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/computeTest.dir/src/main.cpp.o -c /home/marculonis/Desktop/Projects/CPP/CPPDrawing/compute_test/src/main.cpp

CMakeFiles/computeTest.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/computeTest.dir/src/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/marculonis/Desktop/Projects/CPP/CPPDrawing/compute_test/src/main.cpp > CMakeFiles/computeTest.dir/src/main.cpp.i

CMakeFiles/computeTest.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/computeTest.dir/src/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/marculonis/Desktop/Projects/CPP/CPPDrawing/compute_test/src/main.cpp -o CMakeFiles/computeTest.dir/src/main.cpp.s

CMakeFiles/computeTest.dir/src/shader.cpp.o: CMakeFiles/computeTest.dir/flags.make
CMakeFiles/computeTest.dir/src/shader.cpp.o: ../src/shader.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/marculonis/Desktop/Projects/CPP/CPPDrawing/compute_test/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/computeTest.dir/src/shader.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/computeTest.dir/src/shader.cpp.o -c /home/marculonis/Desktop/Projects/CPP/CPPDrawing/compute_test/src/shader.cpp

CMakeFiles/computeTest.dir/src/shader.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/computeTest.dir/src/shader.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/marculonis/Desktop/Projects/CPP/CPPDrawing/compute_test/src/shader.cpp > CMakeFiles/computeTest.dir/src/shader.cpp.i

CMakeFiles/computeTest.dir/src/shader.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/computeTest.dir/src/shader.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/marculonis/Desktop/Projects/CPP/CPPDrawing/compute_test/src/shader.cpp -o CMakeFiles/computeTest.dir/src/shader.cpp.s

CMakeFiles/computeTest.dir/src/texture.cpp.o: CMakeFiles/computeTest.dir/flags.make
CMakeFiles/computeTest.dir/src/texture.cpp.o: ../src/texture.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/marculonis/Desktop/Projects/CPP/CPPDrawing/compute_test/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/computeTest.dir/src/texture.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/computeTest.dir/src/texture.cpp.o -c /home/marculonis/Desktop/Projects/CPP/CPPDrawing/compute_test/src/texture.cpp

CMakeFiles/computeTest.dir/src/texture.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/computeTest.dir/src/texture.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/marculonis/Desktop/Projects/CPP/CPPDrawing/compute_test/src/texture.cpp > CMakeFiles/computeTest.dir/src/texture.cpp.i

CMakeFiles/computeTest.dir/src/texture.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/computeTest.dir/src/texture.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/marculonis/Desktop/Projects/CPP/CPPDrawing/compute_test/src/texture.cpp -o CMakeFiles/computeTest.dir/src/texture.cpp.s

# Object files for target computeTest
computeTest_OBJECTS = \
"CMakeFiles/computeTest.dir/src/main.cpp.o" \
"CMakeFiles/computeTest.dir/src/shader.cpp.o" \
"CMakeFiles/computeTest.dir/src/texture.cpp.o"

# External object files for target computeTest
computeTest_EXTERNAL_OBJECTS =

computeTest: CMakeFiles/computeTest.dir/src/main.cpp.o
computeTest: CMakeFiles/computeTest.dir/src/shader.cpp.o
computeTest: CMakeFiles/computeTest.dir/src/texture.cpp.o
computeTest: CMakeFiles/computeTest.dir/build.make
computeTest: /usr/lib/x86_64-linux-gnu/libOpenGL.so
computeTest: /usr/lib/x86_64-linux-gnu/libGLX.so
computeTest: /usr/lib/x86_64-linux-gnu/libGLU.so
computeTest: /usr/lib/x86_64-linux-gnu/libGLEW.so
computeTest: /usr/lib/x86_64-linux-gnu/libglfw.so.3.3
computeTest: CMakeFiles/computeTest.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/marculonis/Desktop/Projects/CPP/CPPDrawing/compute_test/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable computeTest"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/computeTest.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/computeTest.dir/build: computeTest

.PHONY : CMakeFiles/computeTest.dir/build

CMakeFiles/computeTest.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/computeTest.dir/cmake_clean.cmake
.PHONY : CMakeFiles/computeTest.dir/clean

CMakeFiles/computeTest.dir/depend:
	cd /home/marculonis/Desktop/Projects/CPP/CPPDrawing/compute_test/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/marculonis/Desktop/Projects/CPP/CPPDrawing/compute_test /home/marculonis/Desktop/Projects/CPP/CPPDrawing/compute_test /home/marculonis/Desktop/Projects/CPP/CPPDrawing/compute_test/build /home/marculonis/Desktop/Projects/CPP/CPPDrawing/compute_test/build /home/marculonis/Desktop/Projects/CPP/CPPDrawing/compute_test/build/CMakeFiles/computeTest.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/computeTest.dir/depend

