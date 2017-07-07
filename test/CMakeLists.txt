include_directories("${PROJECT_SOURCE_DIR}/include")

# valgrind, for memory checks
find_program(VALGRIND valgrind)

macro(run_test test_name pass)
	add_executable(${test_name} "${test_name}.c")
	target_link_libraries(${test_name} pega-texto)
	add_test(${test_name} ${test_name} ${ARGN})

	set(_pass ${pass})
	if(_pass STREQUAL "valgrind")
		if(VALGRIND)
			# memcheck with valgrind
			add_test(${test_name}_memcheck valgrind ./${test_name} ${ARGN})
			set_tests_properties(${test_name}_memcheck
				PROPERTIES PASS_REGULAR_EXPRESSION "All heap blocks were freed -- no leaks are possible"
			)
		endif()
	elseif(_pass)
		set_tests_properties(${test_name}
			PROPERTIES PASS_REGULAR_EXPRESSION ${pass}
		)
	endif()
endmacro()

include(CTest)
run_test(JustInclude "PASS")
run_test(DestroysExpr "valgrind")
run_test(DestroysGrammar "valgrind")
run_test(HelloWorldMatcher "PASS")
run_test(PrimaryMatchers "PASS")
run_test(UnaryMatchers "PASS")
run_test(N-aryMatchers "PASS")
run_test(CustomMatchers "PASS")
