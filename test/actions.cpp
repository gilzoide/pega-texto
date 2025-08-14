#include "catch2/catch.hpp"

#include <cstdlib>

#define PT_DATA int
#define PT_DEFINE_SHORTCUTS
#define PEGA_TEXTO_IMPLEMENTATION
#include "pega-texto.h"

PT_DATA parse_int(const char *str, size_t size, int argc, PT_DATA *argv, void *userdata) {
    return atoi(str);
}

PT_DATA sum_ints(const char *str, size_t size, int argc, PT_DATA *argv, void *userdata) {
    int total = 0;
    for (int i = 0; i < argc; i++) {
        total += argv[i];
    }
    return total;
}

TEST_CASE("actions") {
    pt_rule r = PT_RULE(
        ACTION(
            sum_ints,
            SEQ(
                ZERO_OR_MORE(
                    SPACE()
                ),
                ONE_OR_MORE(
                    ACTION(
                        parse_int,
                        ONE_OR_MORE(DIGIT())
                    ),
                    ZERO_OR_MORE(
                        EITHER(
                            B('+'),
                            SPACE()
                        )
                    )
                )
            )
        ),
        NOT(ANY())
    );
    pt_grammar g = { r };

    pt_match_result result = pt_match(g, "123", NULL);
    REQUIRE(result.matched == 3);
    REQUIRE(result.data == 123);
    
    result = pt_match(g, "not a number", NULL);
    REQUIRE(result.matched == PT_NO_MATCH);
    REQUIRE(result.data == 0);
    
    result = pt_match(g, "2 + 6", NULL);
    REQUIRE(result.matched == 5);
    REQUIRE(result.data == 8);

    result = pt_match(g, "2 + 6 + nan", NULL);
    REQUIRE(result.matched == PT_NO_MATCH);
    REQUIRE(result.data == 0);
}
