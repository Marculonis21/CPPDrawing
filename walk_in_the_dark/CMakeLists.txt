cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(walkInTheDark)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)

file(GLOB SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/src/*.cpp)

file(GLOB HEADER_FILES
    ${CMAKE_SOURCE_DIR}/src/*.hpp)

configure_file(src/helpers/RootDir.hpp.in src/helpers/RootDir.hpp)
include_directories(${CMAKE_BINARY_DIR}/src)

add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

find_package(glm REQUIRED)
message(STATUS "glm included at ${GLM_INCLUDE_DIR}")

find_package(glfw3 3.3 REQUIRED)
message(STATUS "glfw3 included at ${GLFW3_INCLUDE_DIR}")

find_package(GLEW 2.0 REQUIRED)
message(STATUS "GLEW included at ${GLEW_INCLUDE_DIR}")

set(LIBS ${GLFW3_LIBRARY} ${OPENGL_LIBRARY})

include_directories(
	"${CMAKE_SOURCE_DIR}/src"
)

target_link_libraries(${PROJECT_NAME} ${LIBS})
target_link_libraries(${PROJECT_NAME} GLEW::GLEW)
target_link_libraries(${PROJECT_NAME} glfw)
