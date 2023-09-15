cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

# Set project name and version
project(QuadTree)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

file(GLOB SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/src/*.cpp)

file(GLOB HEADER_FILES
    ${CMAKE_SOURCE_DIR}/src/*.hpp)

# Find SFML shared libraries
find_package(SFML 2.5 
  COMPONENTS 
    system window graphics network audio REQUIRED
  )

# Compile executable
add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

# Set include directory search paths
include_directories(
	"${CMAKE_SOURCE_DIR}/src"
)

# Link executable to required SFML libraries
target_link_libraries(${PROJECT_NAME} sfml-graphics)

# Install target
# install(TARGETS SFMLBoilerplate DESTINATION bin)

