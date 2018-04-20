cmake_minimum_required(VERSION 3.10)
project(OpenGLES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D CMAKE_C_COMPILER=Clang")
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(${CMAKE_SOURCE_DIR}/lib/opengles/include)
link_directories(${CMAKE_SOURCE_DIR}/lib/opengles/lib)

add_executable(OpenGLES main.c util/gl-util.h util/gl-util.c util/win-util.h util/win-util.c util/native-log.h util/native-log.c)

target_link_libraries(OpenGLES libGLESv2 libEGL)