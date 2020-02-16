file(GLOB src *.c)
add_library(pega-texto ${src})
target_include_directories(pega-texto
	PUBLIC
		$<INSTALL_INTERFACE:include>    
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}
)
set_target_properties(pega-texto PROPERTIES
	VERSION "${PROJECT_VERSION}"
	SOVERSION "${PROJECT_SOVERSION}")

if(NOT WIN32 AND BUILD_SHARED_LIBS AND LINK_SONAME_FILE)
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
include(GNUInstallDirs)
install(TARGETS pega-texto
	EXPORT pega-texto-export
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(EXPORT pega-texto-export
	FILE PegaTextoTargets.cmake
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/PegaTexto)