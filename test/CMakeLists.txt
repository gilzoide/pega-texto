set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}")
find_package(Criterion REQUIRED)

include_directories("${PROJECT_SOURCE_DIR}" ${CRITERION_INCLUDE_DIRS})

link_libraries(${CRITERION_LIBRARIES})

# valgrind, for memory checks
find_program(VALGRIND valgrind)

function(run_test source)
    get_filename_component(test_name ${source} NAME_WE)
	add_executable(${test_name} ${source})
    target_link_libraries(${test_name} ${CRITERION_LIBRARIES})
	add_test(${test_name} ${test_name} ${ARGN})
endfunction()

include(CTest)
run_test("expressions.c")
