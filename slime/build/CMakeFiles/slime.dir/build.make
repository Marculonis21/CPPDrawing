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
CMAKE_SOURCE_DIR = /home/marculonis/Desktop/Projects/CPP/Opengl_drawing/slime

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/marculonis/Desktop/Projects/CPP/Opengl_drawing/slime/build

# Include any dependencies generated for this target.
include CMakeFiles/slime.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/slime.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/slime.dir/flags.make

CMakeFiles/slime.dir/src/main.cpp.o: CMakeFiles/slime.dir/flags.make
CMakeFiles/slime.dir/src/main.cpp.o: ../src/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/marculonis/Desktop/Projects/CPP/Opengl_drawing/slime/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/slime.dir/src/main.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/slime.dir/src/main.cpp.o -c /home/marculonis/Desktop/Projects/CPP/Opengl_drawing/slime/src/main.cpp

CMakeFiles/slime.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/slime.dir/src/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/marculonis/Desktop/Projects/CPP/Opengl_drawing/slime/src/main.cpp > CMakeFiles/slime.dir/src/main.cpp.i

CMakeFiles/slime.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/slime.dir/src/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/marculonis/Desktop/Projects/CPP/Opengl_drawing/slime/src/main.cpp -o CMakeFiles/slime.dir/src/main.cpp.s

CMakeFiles/slime.dir/src/shader.cpp.o: CMakeFiles/slime.dir/flags.make
CMakeFiles/slime.dir/src/shader.cpp.o: ../src/shader.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/marculonis/Desktop/Projects/CPP/Opengl_drawing/slime/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/slime.dir/src/shader.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/slime.dir/src/shader.cpp.o -c /home/marculonis/Desktop/Projects/CPP/Opengl_drawing/slime/src/shader.cpp

CMakeFiles/slime.dir/src/shader.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/slime.dir/src/shader.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/marculonis/Desktop/Projects/CPP/Opengl_drawing/slime/src/shader.cpp > CMakeFiles/slime.dir/src/shader.cpp.i

CMakeFiles/slime.dir/src/shader.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/slime.dir/src/shader.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/marculonis/Desktop/Projects/CPP/Opengl_drawing/slime/src/shader.cpp -o CMakeFiles/slime.dir/src/shader.cpp.s

CMakeFiles/slime.dir/src/texture.cpp.o: CMakeFiles/slime.dir/flags.make
CMakeFiles/slime.dir/src/texture.cpp.o: ../src/texture.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/marculonis/Desktop/Projects/CPP/Opengl_drawing/slime/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/slime.dir/src/texture.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/slime.dir/src/texture.cpp.o -c /home/marculonis/Desktop/Projects/CPP/Opengl_drawing/slime/src/texture.cpp

CMakeFiles/slime.dir/src/texture.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/slime.dir/src/texture.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/marculonis/Desktop/Projects/CPP/Opengl_drawing/slime/src/texture.cpp > CMakeFiles/slime.dir/src/texture.cpp.i

CMakeFiles/slime.dir/src/texture.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/slime.dir/src/texture.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/marculonis/Desktop/Projects/CPP/Opengl_drawing/slime/src/texture.cpp -o CMakeFiles/slime.dir/src/texture.cpp.s

# Object files for target slime
slime_OBJECTS = \
"CMakeFiles/slime.dir/src/main.cpp.o" \
"CMakeFiles/slime.dir/src/shader.cpp.o" \
"CMakeFiles/slime.dir/src/texture.cpp.o"

# External object files for target slime
slime_EXTERNAL_OBJECTS =

slime: CMakeFiles/slime.dir/src/main.cpp.o
slime: CMakeFiles/slime.dir/src/shader.cpp.o
slime: CMakeFiles/slime.dir/src/texture.cpp.o
slime: CMakeFiles/slime.dir/build.make
slime: /usr/lib/x86_64-linux-gnu/libOpenGL.so
slime: /usr/lib/x86_64-linux-gnu/libGLX.so
slime: /usr/lib/x86_64-linux-gnu/libGLU.so
slime: /usr/lib/x86_64-linux-gnu/libGLEW.so
slime: /usr/lib/x86_64-linux-gnu/libglfw.so.3.3
slime: CMakeFiles/slime.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/marculonis/Desktop/Projects/CPP/Opengl_drawing/slime/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable slime"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/slime.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/slime.dir/build: slime

.PHONY : CMakeFiles/slime.dir/build

CMakeFiles/slime.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/slime.dir/cmake_clean.cmake
.PHONY : CMakeFiles/slime.dir/clean

CMakeFiles/slime.dir/depend:
	cd /home/marculonis/Desktop/Projects/CPP/Opengl_drawing/slime/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/marculonis/Desktop/Projects/CPP/Opengl_drawing/slime /home/marculonis/Desktop/Projects/CPP/Opengl_drawing/slime /home/marculonis/Desktop/Projects/CPP/Opengl_drawing/slime/build /home/marculonis/Desktop/Projects/CPP/Opengl_drawing/slime/build /home/marculonis/Desktop/Projects/CPP/Opengl_drawing/slime/build/CMakeFiles/slime.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/slime.dir/depend
