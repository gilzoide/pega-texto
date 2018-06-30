cmake_minimum_required(VERSION 2.6)

set(PROJECT_VERSION_MAJOR 2)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH 0)
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
set(PROJECT_SOVERSION ${PROJECT_VERSION_MAJOR})

if(${CMAKE_VERSION} VERSION_GREATER 3.0)
	cmake_policy(SET CMP0048 NEW)
	project(pega-texto VERSION ${PROJECT_VERSION})
else()
	project(pega-texto)
endif()

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -Wno-parentheses -Wno-unused-function")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2 -DNDEBUG")
set(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} -Os -DNDEBUG")
add_definitions(-DPROJECT_VERSION="${PROJECT_VERSION}")

# Build stuff
option(BUILD_SHARED_LIBS "Build as a shared library" ON)
if(UNIX)
	option(LINK_SONAME_FILE "Create an additional symlink and pkg-config script for library based on it's SONAME" ON)
endif()
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
configure_file("include/pega-texto.h" "pega-texto.h" @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/pega-texto.h" DESTINATION "include")
install(FILES ${headers} DESTINATION "include/${CMAKE_PROJECT_NAME}")


# CPack packaging
include(InstallRequiredSystemLibraries)
set(CPACK_GENERATOR "STGZ;TGZ;TZ")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A runtime engine for Parsing Expression Grammars (PEG) in C")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}")
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_CONTACT "gilzoide@gmail.com")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
include(CPack)
