cmake_minimum_required(VERSION 2.6...3.19)

set(PROJECT_VERSION_MAJOR 4)
set(PROJECT_VERSION_MINOR 1)
set(PROJECT_VERSION_PATCH 0)
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

if(${CMAKE_VERSION} VERSION_GREATER 3.0)
	cmake_policy(SET CMP0048 NEW)
	project(pega-texto VERSION ${PROJECT_VERSION})
else()
	project(pega-texto)
endif()

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wno-missing-braces")

# Test
option(ENABLE_TESTS "Enable tests to be built and run with `make test`" OFF)
if(ENABLE_TESTS)
	enable_testing()
	add_subdirectory(test)
endif()

# Examples
option(BUILD_EXAMPLES "Compile the examples" OFF)
if(BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()

# Benchmark
option(BUILD_BENCHMARK "Compile the benchmark programs" OFF)
if(BUILD_BENCHMARK)
	add_subdirectory(benchmark)
endif()

# Process headers
set(headers "pega-texto.h")
install(FILES ${headers} DESTINATION "include/${CMAKE_PROJECT_NAME}")


# CPack packaging
include(InstallRequiredSystemLibraries)
set(CPACK_GENERATOR "STGZ;TGZ;TZ")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Parsing Expression Grammars (PEG) runtime engine for C")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/UNLICENSE")
set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}")
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_CONTACT "gilzoide@gmail.com")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
include(CPack)
