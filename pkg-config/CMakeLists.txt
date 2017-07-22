include(FindPkgConfig)

if(PKG_CONFIG_FOUND)
	set(_output pega-texto.pc)
	configure_file(pega-texto.pc.in ${_output} @ONLY)
	# find out where to put the .pc file
	execute_process(COMMAND ${PKG_CONFIG_EXECUTABLE} --variable pc_path pkg-config
		OUTPUT_VARIABLE PKG_CONFIG_PC_PATH
		OUTPUT_STRIP_TRAILING_WHITESPACE)
	if(WIN32)
		set(_sep ";")
	else()
		set(_sep ":")
	endif()
	string(REGEX MATCH "[^${_sep}]+" PKG_CONFIG_PC_PATH ${PKG_CONFIG_PC_PATH})
	# and do it!
	install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${_output}" DESTINATION ${PKG_CONFIG_PC_PATH})
endif()
