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
CMAKE_SOURCE_DIR = /home/marculonis/Desktop/Projects/CPP/CPPDrawing/RayMarch/test1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/marculonis/Desktop/Projects/CPP/CPPDrawing/RayMarch/test1/build

# Utility rule file for copy_resources.

# Include the progress variables for this target.
include CMakeFiles/copy_resources.dir/progress.make

CMakeFiles/copy_resources:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/marculonis/Desktop/Projects/CPP/CPPDrawing/RayMarch/test1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Copying shaders to binary dir"
	/usr/bin/cmake -E copy_directory /home/marculonis/Desktop/Projects/CPP/CPPDrawing/RayMarch/test1/src/shaders /home/marculonis/Desktop/Projects/CPP/CPPDrawing/RayMarch/test1/build/src/shaders

copy_resources: CMakeFiles/copy_resources
copy_resources: CMakeFiles/copy_resources.dir/build.make

.PHONY : copy_resources

# Rule to build all files generated by this target.
CMakeFiles/copy_resources.dir/build: copy_resources

.PHONY : CMakeFiles/copy_resources.dir/build

CMakeFiles/copy_resources.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/copy_resources.dir/cmake_clean.cmake
.PHONY : CMakeFiles/copy_resources.dir/clean

CMakeFiles/copy_resources.dir/depend:
	cd /home/marculonis/Desktop/Projects/CPP/CPPDrawing/RayMarch/test1/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/marculonis/Desktop/Projects/CPP/CPPDrawing/RayMarch/test1 /home/marculonis/Desktop/Projects/CPP/CPPDrawing/RayMarch/test1 /home/marculonis/Desktop/Projects/CPP/CPPDrawing/RayMarch/test1/build /home/marculonis/Desktop/Projects/CPP/CPPDrawing/RayMarch/test1/build /home/marculonis/Desktop/Projects/CPP/CPPDrawing/RayMarch/test1/build/CMakeFiles/copy_resources.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/copy_resources.dir/depend

