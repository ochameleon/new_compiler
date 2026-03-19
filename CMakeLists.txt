cmake_minimum_required(VERSION 3.16)

project(OChameleon LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable warnings
if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Collect source files
file(GLOB_RECURSE SOURCES
    main.cpp
    lib/*.cpp
)

# Create executable
add_executable(ochameleon ${SOURCES})

# Include directories
target_include_directories(ochameleon PRIVATE
    ${PROJECT_SOURCE_DIR}/lib
)

# Put binary in /bin
set_target_properties(ochameleon PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
)
