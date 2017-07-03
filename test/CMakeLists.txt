include_directories(${PROJECT_SOURCE_DIR})

macro(run_test test_name pass)
	add_executable(${test_name} "${test_name}.c")
	target_link_libraries(${test_name} pega-texto)
	add_test(${test_name} ${test_name} ${ARGN})
	set(_pass ${pass})
	if(_pass)
		set_tests_properties(${test_name}
			PROPERTIES PASS_REGULAR_EXPRESSION ${pass}
		)
	endif()
endmacro()

include(CTest)
run_test(JustInclude "PASS")
run_test(Destroys OFF)
