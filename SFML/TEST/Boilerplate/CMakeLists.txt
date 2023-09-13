cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

# Set project name and version
project(SFMLBoilerplate VERSION 0.1)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Enable debug symbols by default
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug 
    CACHE STRING 
      "Choose the type of build (Debug or Release)" 
      FORCE
    )
endif()

# Generate config.h
configure_file(config.h.in config.h)

# Find SFML shared libraries
find_package(SFML 2.5 
  COMPONENTS 
    system window graphics network audio REQUIRED
  )

# Compile executable
add_executable(SFMLBoilerplate main.cpp)

# Set include directory search paths
target_include_directories(SFMLBoilerplate 
  PRIVATE
  "${PROJECT_BINARY_DIR}"
  )

# Link executable to required SFML libraries
target_link_libraries(SFMLBoilerplate sfml-graphics)

# Install target
install(TARGETS SFMLBoilerplate DESTINATION bin)

