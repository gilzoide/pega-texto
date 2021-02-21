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
        cr_assert_eq(res.matched, 1);

        str[0] = i + 1;
        res = pt_match(g, str, NULL);
        cr_assert_eq(res.matched, PT_NO_MATCH);
    }
}
