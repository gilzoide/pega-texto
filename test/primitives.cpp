#include <catch2/catch.hpp>

#define PT_DEFINE_SHORTCUTS
#define PEGA_TEXTO_IMPLEMENTATION
#include "pega-texto.h"

TEST_CASE("pega-texto expressions") {
    for(int i = 0; i < 127; i++) {
        pt_expr e = B(i);
        pt_rule r = PT_RULE(e);
        pt_grammar g = { r };
        char str[] = { (char) i, 0 };
        pt_match_result res = pt_match(g, str, NULL);
        REQUIRE(res.matched == 1);

        str[0] = i + 1;
        res = pt_match(g, str, NULL);
        REQUIRE(res.matched == PT_NO_MATCH);
    }
}

TEST_CASE("pt_expr, literal") {
    pt_rule r = PT_RULE(L("hello"));
    pt_grammar g = { r };

    pt_match_result res = pt_match(g, "hello", NULL);
    REQUIRE(res.matched == 5);
    res = pt_match(g, "hello world", NULL);
    REQUIRE(res.matched == 5);
    res = pt_match(g, "world", NULL);
    REQUIRE(res.matched == PT_NO_MATCH);
    res = pt_match(g, "hell", NULL);
    REQUIRE(res.matched == PT_NO_MATCH);
    res = pt_match(g, " hello", NULL);
    REQUIRE(res.matched == PT_NO_MATCH);
    res = pt_match(g, "HELLO", NULL);
    REQUIRE(res.matched == PT_NO_MATCH);
    res = pt_match(g, "heLlo", NULL);
    REQUIRE(res.matched == PT_NO_MATCH);
}

TEST_CASE("pt_expr, case_insensitive") {
    pt_rule r = PT_RULE(I("hello"));
    pt_grammar g = { r };

    pt_match_result res = pt_match(g, "hello", NULL);
    REQUIRE(res.matched == 5);
    res = pt_match(g, "helloworld", NULL);
    REQUIRE(res.matched == 5);
    res = pt_match(g, "world", NULL);
    REQUIRE(res.matched == PT_NO_MATCH);
    res = pt_match(g, "hell", NULL);
    REQUIRE(res.matched == PT_NO_MATCH);
    res = pt_match(g, " hello", NULL);
    REQUIRE(res.matched == PT_NO_MATCH);
    res = pt_match(g, "HELLO", NULL);
    REQUIRE(res.matched == 5);
    res = pt_match(g, "HeLlO wOrLd", NULL);
    REQUIRE(res.matched == 5);
}

int only_zeros(int c) { return c == 0; }
TEST_CASE("pt_expr, character_class") {
    {
        pt_rule r = PT_RULE(DIGIT());
        pt_grammar g = { r };
        for(char i = '0'; i <= '9'; i++) {
            char str[] = { i, 0 };
            pt_match_result res = pt_match(g, str, NULL);
            REQUIRE(res.matched == 1);
        }
        for(char i = 'a'; i <= 'z'; i++) {
            char str[] = { i, 0 };
            pt_match_result res = pt_match(g, str, NULL);
            REQUIRE(res.matched == PT_NO_MATCH);
        }
    }

    {
        pt_rule r = PT_RULE(ALPHA());
        pt_grammar g = { r };
        for(char i = '0'; i <= '9'; i++) {
            char str[] = { i, 0 };
            pt_match_result res = pt_match(g, str, NULL);
            REQUIRE(res.matched == PT_NO_MATCH);
        }
        for(char i = 'a'; i <= 'z'; i++) {
            char str[] = { i, 0 };
            pt_match_result res = pt_match(g, str, NULL);
            REQUIRE(res.matched == 1);
        }
    }

    {
        pt_rule r = PT_RULE(ALNUM());
        pt_grammar g = { r };
        for(char i = '0'; i <= '9'; i++) {
            char str[] = { i, 0 };
            pt_match_result res = pt_match(g, str, NULL);
            REQUIRE(res.matched == 1);
        }
        for(char i = 'a'; i <= 'z'; i++) {
            char str[] = { i, 0 };
            pt_match_result res = pt_match(g, str, NULL);
            REQUIRE(res.matched == 1);
        }
    }

    {
        pt_rule r = PT_RULE(CLASS(only_zeros));
        pt_grammar g = { r };
        for(int i = 1; i < 128; i++) {
            char str[] = { (char) i, 0 };
            pt_match_result res = pt_match(g, str, NULL);
            REQUIRE(res.matched == PT_NO_MATCH);
        }
        pt_match_result res = pt_match(g, "", NULL);
        REQUIRE(res.matched == 1);
    }
}

TEST_CASE("pt_expr, set") {
    pt_rule r = PT_RULE(S("abc"));
    pt_grammar g = { r };

    pt_match_result res = pt_match(g, "alo", NULL);
    REQUIRE(res.matched == 1);
    res = pt_match(g, "belo dia", NULL);
    REQUIRE(res.matched == 1);
    res = pt_match(g, "concorda?", NULL);
    REQUIRE(res.matched == 1);

    res = pt_match(g, "não", NULL);
    REQUIRE(res.matched == PT_NO_MATCH);
    res = pt_match(g, "diacho", NULL);
    REQUIRE(res.matched == PT_NO_MATCH);
    res = pt_match(g, ".", NULL);
    REQUIRE(res.matched == PT_NO_MATCH);
    res = pt_match(g, "Amanhã talvez", NULL);
    REQUIRE(res.matched == PT_NO_MATCH);
    res = pt_match(g, "", NULL);
    REQUIRE(res.matched == PT_NO_MATCH);
}

TEST_CASE("pt_expr, range") {
    pt_rule r = PT_RULE(R('a', 'c'));
    pt_grammar g = { r };

    pt_match_result res = pt_match(g, "alo", NULL);
    REQUIRE(res.matched == 1);
    res = pt_match(g, "belo dia", NULL);
    REQUIRE(res.matched == 1);
    res = pt_match(g, "concorda?", NULL);
    REQUIRE(res.matched == 1);

    res = pt_match(g, "não", NULL);
    REQUIRE(res.matched == PT_NO_MATCH);
    res = pt_match(g, "diacho", NULL);
    REQUIRE(res.matched == PT_NO_MATCH);
    res = pt_match(g, ".", NULL);
    REQUIRE(res.matched == PT_NO_MATCH);
    res = pt_match(g, "Amanhã talvez", NULL);
    REQUIRE(res.matched == PT_NO_MATCH);
    res = pt_match(g, "", NULL);
    REQUIRE(res.matched == PT_NO_MATCH);
}

TEST_CASE("pt_expr, any") {
    pt_rule r = PT_RULE(ANY());
    pt_grammar g = { r };

    pt_match_result res = pt_match(g, "hello!", NULL);
    REQUIRE(res.matched == 1);
    res = pt_match(g, " ", NULL);
    REQUIRE(res.matched == 1);
    res = pt_match(g, "\3", NULL);
    REQUIRE(res.matched == 1);
    res = pt_match(g, "", NULL);
    REQUIRE(res.matched == PT_NO_MATCH);
}
