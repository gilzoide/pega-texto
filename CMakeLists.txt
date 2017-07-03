cmake_minimum_required(VERSION 2.6)
project(pega-texto)

# Build
add_library(lib SHARED pega-texto.h pega-texto.c)

# Test
enable_testing()
add_subdirectory(test)

# Install TODO
