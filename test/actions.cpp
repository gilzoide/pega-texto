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
    // EXPR <- SPACE NUMBER (SPACE '+' (NUMBER / error))*
    // NUMBER <- \d+
    // SPACE <- \s*
    enum rules {
        R_EXPR,
        R_NUMBER,
        R_SPACE,
    };
    pt_rule r_expr = PT_RULE(
        ACTION(
            sum_ints,
            SEQ(
                CALL(R_SPACE),
                CALL(R_NUMBER),
                ZERO_OR_MORE(
                    CALL(R_SPACE),
                    B('+'),
                    CALL(R_SPACE),
                    EITHER(
                        CALL(R_NUMBER),
                        ERROR(NULL)  // Syntax error: after `+` a number is required!
                    ),
                    CALL(R_SPACE)
                )
            )
        ),
        NOT(ANY())
    );
    pt_rule r_number = PT_RULE(
        ACTION(
            parse_int,
            ONE_OR_MORE(DIGIT())
        )
    );
    pt_rule r_space = PT_RULE(
        ZERO_OR_MORE(SPACE())
    );
    pt_grammar g = {
        [R_EXPR] = r_expr,
        [R_NUMBER] = r_number,
        [R_SPACE] = r_space,
    };

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
    REQUIRE(result.matched == PT_MATCHED_ERROR);
    REQUIRE(result.data == 0);

    // test matching with max size
    result = pt_match_n(g, "2 + 6 + nan", 0, NULL);
    REQUIRE(result.matched == PT_NO_MATCH);
    REQUIRE(result.data == 0);

    result = pt_match_n(g, "2 + 6 + nan", 1, NULL);
    REQUIRE(result.matched == 1);
    REQUIRE(result.data == 2);

    result = pt_match_n(g, "2 + 6 + nan", 5, NULL);
    REQUIRE(result.matched == 5);
    REQUIRE(result.data == 8);
    
    result = pt_match_n(g, "2 + 6 + nan", 6, NULL);
    REQUIRE(result.matched == 6);
    REQUIRE(result.data == 8);
    
    result = pt_match_n(g, "2 + 6 + nan", 7, NULL);
    REQUIRE(result.matched == PT_MATCHED_ERROR);
    REQUIRE(result.data == 0);
}
