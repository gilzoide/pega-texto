include(FindPkgConfig)

# only build/install if `pkg-config` is available and if building shared lib
if(PKG_CONFIG_FOUND AND BUILD_SHARED_LIBS)
	# find out where to put the .pc file
	execute_process(COMMAND ${PKG_CONFIG_EXECUTABLE} --variable pc_path pkg-config
		OUTPUT_VARIABLE PKG_CONFIG_PC_PATH
		OUTPUT_STRIP_TRAILING_WHITESPACE)
	if(WIN32)
		set(_sep ";")
	else()
		set(_sep ":")
	endif()
	string(REGEX MATCH "[^${_sep}]+" pc-path ${PKG_CONFIG_PC_PATH})

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
