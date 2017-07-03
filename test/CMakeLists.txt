include_directories(${PROJECT_SOURCE_DIR})
add_executable(main main.c)

include(CTest)
add_test(JustInclude main)
