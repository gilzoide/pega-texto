# valgrind, for memory checks
#find_program(VALGRIND valgrind)

function(run_test source)
    get_filename_component(test_name ${source} NAME_WE)
	add_executable(${test_name} main.cpp ${source})
    target_include_directories(${test_name} PUBLIC ${CMAKE_SOURCE_DIR} catch2/single_include)
	add_test(${test_name} ${test_name} ${ARGN})
endfunction()

include(CTest)
run_test("primitives.cpp")
