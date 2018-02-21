include_directories("${PROJECT_SOURCE_DIR}/include")

file(GLOB src *.c)
add_library(pega-texto ${src})
set_target_properties(pega-texto PROPERTIES
	VERSION "${PROJECT_VERSION}"
	SOVERSION "${PROJECT_SOVERSION}")

if(UNIX AND LINK_SONAME_FILE)
	if(APPLE)
		set(lib-ext dylib)
	else()
		set(lib-ext so)
	endif()
	set(link-name "libpega-texto${PROJECT_VERSION_MAJOR}.${lib-ext}")
	add_custom_command(TARGET pega-texto POST_BUILD COMMAND ${CMAKE_COMMAND} -E create_symlink $<TARGET_SONAME_FILE_NAME:pega-texto> ${link-name})
	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${link-name} DESTINATION lib)
endif()

# Install library
install(TARGETS pega-texto
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib)

