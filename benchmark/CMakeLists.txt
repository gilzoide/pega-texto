include_directories("${PROJECT_SOURCE_DIR}")

# The grammar input
configure_file(grammar.txt grammar.txt COPYONLY)

# Pega-texto
add_executable(PegaTexto pega-texto.c)

# Flex + Bison
find_package(FLEX)
find_package(BISON)
if (FLEX_FOUND AND BISON_FOUND)
	BISON_TARGET(BenchmarkParser bison.y ${CMAKE_CURRENT_BINARY_DIR}/parser.c)
	FLEX_TARGET(BenchmarkLexer flex.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.c)
	ADD_FLEX_BISON_DEPENDENCY(BenchmarkLexer BenchmarkParser)
	include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
	add_executable(FlexBison
		${BISON_BenchmarkParser_OUTPUTS}
		${FLEX_BenchmarkLexer_OUTPUTS})
endif()

# LPeg
configure_file(lpeg.lua lpeg.lua COPYONLY)

# Leg
add_custom_command(OUTPUT peg.c
	COMMAND peg -opeg.c ${CMAKE_CURRENT_SOURCE_DIR}/peg.peg
	DEPENDS peg-main.c peg.peg)
add_executable(Peg peg-main.c peg.c)

