cmake_minimum_required(VERSION 2.6)
project(pega-texto)

# Compile
add_subdirectory(src)

# Test
option(ENABLE_TESTS "Enable tests to be built and run with `make test`" OFF)
if(ENABLE_TESTS)
	enable_testing()
	add_subdirectory(test)
endif()

# Examples
option(COMPILE_EXAMPLES "Compile the examples" OFF)
if(COMPILE_EXAMPLES)
	add_subdirectory(examples)
endif()

# Install headers
file(GLOB headers "include/*.h")
install(FILES ${headers} DESTINATION "include/${CMAKE_PROJECT_NAME}")
