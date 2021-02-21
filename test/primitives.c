#include <criterion/criterion.h>

#define PT_DEFINE_SHORTCUTS
#define PEGA_TEXTO_IMPLEMENTATION
#include "pega-texto.h"

Test(pt_expr, element) {
    for(int i = 0; i < 127; i++) {
        pt_expr e = B(i);
        pt_rule r = PT_RULE(e);
        pt_grammar g = { r };
        char str[] = { (char) i, 0 };
        pt_match_result res = pt_match(g, str, NULL);
        cr_expect_eq(res.matched, 1);

        str[0] = i + 1;
        res = pt_match(g, str, NULL);
        cr_expect_eq(res.matched, PT_NO_MATCH);
    }
}

Test(pt_expr, literal) {
    pt_rule r = PT_RULE(L("hello"));
    pt_grammar g = { r };

    pt_match_result res = pt_match(g, "hello", NULL);
    cr_expect_eq(res.matched, 5);
    res = pt_match(g, "hello world", NULL);
    cr_expect_eq(res.matched, 5);
    res = pt_match(g, "world", NULL);
    cr_expect_eq(res.matched, PT_NO_MATCH);
    res = pt_match(g, "hell", NULL);
    cr_expect_eq(res.matched, PT_NO_MATCH);
    res = pt_match(g, " hello", NULL);
    cr_expect_eq(res.matched, PT_NO_MATCH);
    res = pt_match(g, "HELLO", NULL);
    cr_expect_eq(res.matched, PT_NO_MATCH);
    res = pt_match(g, "heLlo", NULL);
    cr_expect_eq(res.matched, PT_NO_MATCH);
}

Test(pt_expr, case_insensitive) {
    pt_rule r = PT_RULE(I("hello"));
    pt_grammar g = { r };

    pt_match_result res = pt_match(g, "hello", NULL);
    cr_expect_eq(res.matched, 5);
    res = pt_match(g, "helloworld", NULL);
    cr_expect_eq(res.matched, 5);
    res = pt_match(g, "world", NULL);
    cr_expect_eq(res.matched, PT_NO_MATCH);
    res = pt_match(g, "hell", NULL);
    cr_expect_eq(res.matched, PT_NO_MATCH);
    res = pt_match(g, " hello", NULL);
    cr_expect_eq(res.matched, PT_NO_MATCH);
    res = pt_match(g, "HELLO", NULL);
    cr_expect_eq(res.matched, 5);
    res = pt_match(g, "HeLlO wOrLd", NULL);
    cr_expect_eq(res.matched, 5);
}

int only_zeros(int c) { return c == 0; }
Test(pt_expr, character_class) {
    {
        pt_rule r = PT_RULE(DIGIT());
        pt_grammar g = { r };
        for(char i = '0'; i <= '9'; i++) {
            char str[] = { i, 0 };
            pt_match_result res = pt_match(g, str, NULL);
            cr_expect_eq(res.matched, 1);
        }
        for(char i = 'a'; i <= 'z'; i++) {
            char str[] = { i, 0 };
            pt_match_result res = pt_match(g, str, NULL);
            cr_expect_eq(res.matched, PT_NO_MATCH);
        }
    }

    {
        pt_rule r = PT_RULE(ALPHA());
        pt_grammar g = { r };
        for(char i = '0'; i <= '9'; i++) {
            char str[] = { i, 0 };
            pt_match_result res = pt_match(g, str, NULL);
            cr_expect_eq(res.matched, PT_NO_MATCH);
        }
        for(char i = 'a'; i <= 'z'; i++) {
            char str[] = { i, 0 };
            pt_match_result res = pt_match(g, str, NULL);
            cr_expect_eq(res.matched, 1);
        }
    }

    {
        pt_rule r = PT_RULE(ALNUM());
        pt_grammar g = { r };
        for(char i = '0'; i <= '9'; i++) {
            char str[] = { i, 0 };
            pt_match_result res = pt_match(g, str, NULL);
            cr_expect_eq(res.matched, 1);
        }
        for(char i = 'a'; i <= 'z'; i++) {
            char str[] = { i, 0 };
            pt_match_result res = pt_match(g, str, NULL);
            cr_expect_eq(res.matched, 1);
        }
    }

    {
        pt_rule r = PT_RULE(CLASS(only_zeros));
        pt_grammar g = { r };
        for(int i = 1; i < 128; i++) {
            char str[] = { (char) i, 0 };
            pt_match_result res = pt_match(g, str, NULL);
            cr_expect_eq(res.matched, PT_NO_MATCH);
        }
        pt_match_result res = pt_match(g, "", NULL);
        cr_expect_eq(res.matched, 1);
    }
}

Test(pt_expr, set) {
    pt_rule r = PT_RULE(S("abc"));
    pt_grammar g = { r };

    pt_match_result res = pt_match(g, "alo", NULL);
    cr_expect_eq(res.matched, 1);
    res = pt_match(g, "belo dia", NULL);
    cr_expect_eq(res.matched, 1);
    res = pt_match(g, "concorda?", NULL);
    cr_expect_eq(res.matched, 1);

    res = pt_match(g, "não", NULL);
    cr_expect_eq(res.matched, PT_NO_MATCH);
    res = pt_match(g, "diacho", NULL);
    cr_expect_eq(res.matched, PT_NO_MATCH);
    res = pt_match(g, ".", NULL);
    cr_expect_eq(res.matched, PT_NO_MATCH);
    res = pt_match(g, "Amanhã talvez", NULL);
    cr_expect_eq(res.matched, PT_NO_MATCH);
    res = pt_match(g, "", NULL);
    cr_expect_eq(res.matched, PT_NO_MATCH);
}

Test(pt_expr, range) {
    pt_rule r = PT_RULE(R('a', 'c'));
    pt_grammar g = { r };

    pt_match_result res = pt_match(g, "alo", NULL);
    cr_expect_eq(res.matched, 1);
    res = pt_match(g, "belo dia", NULL);
    cr_expect_eq(res.matched, 1);
    res = pt_match(g, "concorda?", NULL);
    cr_expect_eq(res.matched, 1);

    res = pt_match(g, "não", NULL);
    cr_expect_eq(res.matched, PT_NO_MATCH);
    res = pt_match(g, "diacho", NULL);
    cr_expect_eq(res.matched, PT_NO_MATCH);
    res = pt_match(g, ".", NULL);
    cr_expect_eq(res.matched, PT_NO_MATCH);
    res = pt_match(g, "Amanhã talvez", NULL);
    cr_expect_eq(res.matched, PT_NO_MATCH);
    res = pt_match(g, "", NULL);
    cr_expect_eq(res.matched, PT_NO_MATCH);
}

Test(pt_expr, any) {
    pt_rule r = PT_RULE(ANY());
    pt_grammar g = { r };

    pt_match_result res = pt_match(g, "hello!", NULL);
    cr_expect_eq(res.matched, 1);
    res = pt_match(g, " ", NULL);
    cr_expect_eq(res.matched, 1);
    res = pt_match(g, "\3", NULL);
    cr_expect_eq(res.matched, 1);
    res = pt_match(g, "", NULL);
    cr_expect_eq(res.matched, PT_NO_MATCH);
}

