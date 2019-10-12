#include <pega-texto.h>
#include <pega-texto/macro-on.h>

#include <iostream>

using namespace std;

const char map1[] =
R"(Hi there folks, are you looking for Wallison?
He is a really nice guy I know.
Or maybe you just want to find him.
Yes, WaLLy, here at line 4, column 6.
)";

struct Position {
	const char *original_str;
	int line {1};
	size_t last {0};
};

pt_data count_lines(const char *str, size_t end, int argc, pt_data *argv, void *data) {
	Position *pos = reinterpret_cast<Position *>(data);
	pos->line++;
	pos->last = str - pos->original_str;
	return PT_NULL_DATA;
}

int main() {
	pt_rule rules[] = {
		{ "Where", SEQ(Q(OR(L_(count_lines, "\n"), BUT(V("Wally"))), 0), V("Wally")) },
		{ "Wally", I("wally") },
		{ NULL, NULL },
	};
	pt_grammar g;
	pt_init_grammar(&g, rules, 0);
	pt_validate_grammar(&g, PT_VALIDATE_ABORT);

	Position pos;
	pos.original_str = map1;
	pt_match_options opts = {&pos};
	pt_match_result res = pt_match_grammar(&g, map1, &opts);
	if(res.matched >= 0) {
		cout << "Found ";
		cout.write(map1 + res.matched - 5, 5) << " @ line " << pos.line
				<< ", column " << (res.matched - 5 - pos.last) << endl;
		cout << "PASS" << endl;
	}
	else {
		cout << "FAIL" << endl;
	}

	pt_release_grammar(&g);
	return 0;
}

