include(FindPkgConfig)

# only build/install if `pkg-config` is available and if building shared lib
if(PKG_CONFIG_FOUND AND BUILD_SHARED_LIBS)
	set(pc-path "lib/pkgconfig")

	# configure main .pc file
	set(_output pega-texto.pc)
	configure_file(pega-texto.pc.in ${_output} @ONLY)
	install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${_output}" DESTINATION ${pc-path})

	# configure SONAME specific .pc file
	if(LINK_SONAME_FILE)
		set(_output "pega-texto${PROJECT_SOVERSION}.pc")
		set(WITH_SOVERSION ${PROJECT_SOVERSION})
		configure_file(pega-texto.pc.in ${_output} @ONLY)
		install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${_output}" DESTINATION ${pc-path})
	endif()
endif()
