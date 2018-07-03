# FindPegaTexto
# -------------
# Locate pega-texto library. This module defines:
#
#   PEGA_TEXTO_FOUND         - true if pega-texto library was found
#   PEGA_TEXTO_LIBRARIES     - pega-texto library
#   PEGA_TEXTO_INCLUDE_DIRS  - where to find pega-texto.h
#   PEGA_TEXTO_VERSION       - the version of pega-texto found

find_path(PEGA_TEXTO_INCLUDE_DIRS
	pega-texto.h
	PATH_SUFFIXES pega-texto/include)

find_library(PEGA_TEXTO_LIBRARIES
	NAMES pega-texto pega-texto2 pega-texto1
	PATH_SUFFIXES pega-texto/lib)

# find library version in header file
if(PEGA_TEXTO_INCLUDE_DIRS)
	file(STRINGS "${PEGA_TEXTO_INCLUDE_DIRS}/pega-texto.h" _version_line REGEX "PT_VERSION")
	string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+" PEGA_TEXTO_VERSION ${_version_line})
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PegaTexto
	REQUIRED_VARS PEGA_TEXTO_LIBRARIES PEGA_TEXTO_INCLUDE_DIRS
	VERSION_VAR PEGA_TEXTO_VERSION)

mark_as_advanced(PEGA_TEXTO_LIBRARIES PEGA_TEXTO_INCLUDE_DIRS)
