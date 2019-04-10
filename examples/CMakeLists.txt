include_directories("${PROJECT_SOURCE_DIR}/include")

# file(GLOB examples *.c)
set(examples Compile-test.c)

foreach(src IN LISTS examples)
	get_filename_component(_name ${src} NAME_WE)
	add_executable(${_name} ${src})
	target_link_libraries(${_name} $<TARGET_LINKER_FILE:pega-texto>)
endforeach()
