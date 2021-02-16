include_directories("${PROJECT_SOURCE_DIR}")

# file(GLOB examples *.c)
set(examples readme.c)

foreach(src IN LISTS examples)
	get_filename_component(_name ${src} NAME_WE)
	add_executable(${_name} ${src})
endforeach()
