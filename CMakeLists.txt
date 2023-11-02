cmake_minimum_required(VERSION 3.0)
project(MyProject)

# Set the Clang compiler
set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")

# Add your source files
set(SOURCES
    src/lib.c
    src/main.c
)

# Include the header files
include_directories(src)

# Create an executable from the source files
add_executable(my_executable ${SOURCES})
