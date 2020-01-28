include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/vm")

file(GLOB src *.c)
add_library(pega-texto-vm ${src})
set_target_properties(pega-texto-vm PROPERTIES
	VERSION "${PROJECT_VERSION}"
	SOVERSION "${PROJECT_SOVERSION}")

#####  Assembler  #####
file(GLOB src "assembler/*.c")
add_executable(pega-texto-assembler ${src})
set_target_properties(pega-texto-assembler PROPERTIES
	VERSION "${PROJECT_VERSION}"
	SOVERSION "${PROJECT_SOVERSION}")

target_link_libraries(pega-texto-assembler pega-texto-vm)

#####  Compiler  #####
file(GLOB src "compiler/*.c")
add_executable(pega-texto-compiler ${src})
set_target_properties(pega-texto-compiler PROPERTIES
	VERSION "${PROJECT_VERSION}"
	SOVERSION "${PROJECT_SOVERSION}")

target_link_libraries(pega-texto-compiler pega-texto-vm pega-texto)

# if(NOT WIN32 AND BUILD_SHARED_LIBS AND LINK_SONAME_FILE)
	# if(APPLE)
		# set(lib-ext dylib)
	# else()
		# set(lib-ext so)
	# endif()
	# set(link-name "libpega-texto${PROJECT_VERSION_MAJOR}.${lib-ext}")
	# add_custom_command(TARGET pega-texto POST_BUILD COMMAND ${CMAKE_COMMAND} -E create_symlink $<TARGET_SONAME_FILE_NAME:pega-texto> ${link-name})
	# install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${link-name} DESTINATION lib)
# endif()
#
# # Install library
# install(TARGETS pega-texto
	# LIBRARY DESTINATION lib
	# ARCHIVE DESTINATION lib)
