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
		set_tests_properties(${test_name} PROPERTIES
			PASS_REGULAR_EXPRESSION "PASS"
			FAIL_REGULAR_EXPRESSION "FAIL"
		)
	endif()
endmacro()

include(CTest)
run_test(JustInclude "PASS")
# memory management
run_test(DestroysExpr "valgrind")
run_test(DestroysGrammar "valgrind")
# test each operation apart
run_test(HelloWorldMatcher "valgrind")
run_test(PrimaryMatchers "PASS")
run_test(UnaryMatchers "PASS")
run_test(N-aryMatchers "PASS")
run_test(CustomMatchers "PASS")
# actions
run_test(SuccessAction "PASS")
run_test(CountAndSumNumbers "PASS")
# validation
run_test(GrammarValidation "PASS")
# full tests
run_test(MatchingParens "PASS")
