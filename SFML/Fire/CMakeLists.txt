cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

# Set project name and version
project(Fire)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_subdirectory(dependencies)

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

file(GLOB SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/src/*.cpp)

file(GLOB HEADER_FILES
    ${CMAKE_SOURCE_DIR}/src/*.hpp)

# configure_file(src/helpers/RootDir.hpp.in src/helpers/RootDir.hpp)
include_directories(${CMAKE_BINARY_DIR}/src)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# Find SFML shared libraries
find_package(SFML 2.5 
  COMPONENTS 
    system window graphics network audio REQUIRED
  )

# Compile executable
add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

if (UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE pthread)
endif (UNIX)

# Set include directory search paths
include_directories(
	"${CMAKE_SOURCE_DIR}/src"
)

# Link executable to required SFML libraries
target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics ImGui-SFML::ImGui-SFML)

