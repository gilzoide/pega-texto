include_directories("${PROJECT_SOURCE_DIR}/include")

file(GLOB src *.c)
add_library(pega-texto ${src})

# Install library
install(TARGETS pega-texto
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib)
