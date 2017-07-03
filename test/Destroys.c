#define PEGA_TEXTO_MACROS
#include <pega-texto.h>
#include <ctype.h>

int main() {
	pt_expr *e[] = {
		Seq(L("oi"), Vi(1), Q(Vi(2), 1)),
		F(isspace), // space
		F(isalpha), // letters
	};

	int i;
	for(i = 0; i < sizeof(e) / sizeof(pt_expr *); i++) {
		pt_destroy_expr(e[i]);
	}
	return 0;
}
