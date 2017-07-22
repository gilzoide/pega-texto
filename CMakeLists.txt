cmake_minimum_required(VERSION 2.6)
set(PROJECT_VERSION 0.1.0)
if(${CMAKE_VERSION} VERSION_GREATER 3.0)
	cmake_policy(SET CMP0048 NEW)
	project(pega-texto VERSION ${PROJECT_VERSION})
else()
	project(pega-texto)
endif()

# Build stuff
add_subdirectory(src)
add_subdirectory(pkg-config)

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
file(GLOB headers "include/pega-texto/*.h")
install(FILES ${headers} DESTINATION "include/${CMAKE_PROJECT_NAME}")
