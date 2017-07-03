cmake_minimum_required(VERSION 2.6)
project(pega-texto)

# Compile
add_subdirectory(src)

# Test
enable_testing()
add_subdirectory(test)

# Install headers
file(GLOB headers "include/*.h")
install(FILES ${headers} DESTINATION "include/${CMAKE_PROJECT_NAME}")
