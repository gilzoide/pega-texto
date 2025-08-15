#include "catch2/catch.hpp"

#include <cstdlib>

#define PT_ELEMENT_TYPE const char16_t
#define PT_DEFINE_SHORTCUTS
#define PEGA_TEXTO_IMPLEMENTATION
#include "pega-texto.h"

TEST_CASE("literal char16_t") {
    char16_t str[] = u"hello UTF-16";
    size_t str_len = (sizeof(str) - 1) / sizeof(char16_t);
    pt_rule r = PT_RULE(L(str));
    pt_grammar g = { r };

    pt_match_result result = pt_match(g, str, NULL);
    REQUIRE(result.matched == str_len);
    
    result = pt_match(g, u"something else", NULL);
    REQUIRE(result.matched == PT_NO_MATCH);
}

TEST_CASE("case insensitive char16_t") {
    char16_t str[] = u"hello UTF-16";
    size_t str_len = (sizeof(str) - 1) / sizeof(char16_t);
    pt_rule r = PT_RULE(I(str));
    pt_grammar g = { r };

    pt_match_result result = pt_match(g, u"Hello utf-16", NULL);
    REQUIRE(result.matched == str_len);

    result = pt_match(g, u"something else", NULL);
    REQUIRE(result.matched == PT_NO_MATCH);
}

TEST_CASE("set char16_t") {
    char16_t str[] = u"hello UTF-16";
    pt_rule r = PT_RULE(
        ONE_OR_MORE(
            S(u"Hhel")
        )
    );
    pt_grammar g = { r };

    pt_match_result result = pt_match(g, u"Hello utf-16", NULL);
    REQUIRE(result.matched == 4);

    result = pt_match(g, u"something else", NULL);
    REQUIRE(result.matched == PT_NO_MATCH);
}
