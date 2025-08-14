#include "catch2/catch.hpp"

#define PT_DEFINE_SHORTCUTS
#define PEGA_TEXTO_IMPLEMENTATION
#include "pega-texto.h"

static void *malloc_error(size_t size, void *userdata) {
    return NULL;
}
static void *realloc_error(void *ptr, size_t size, void *userdata) {
    return NULL;
}
static void free_noop(void *ptr, void *userdata) {
}

TEST_CASE("memory error results") {
    pt_match_options opts;
    opts.malloc = &malloc_error;
    opts.realloc = &realloc_error;
    opts.free = &free_noop;

    pt_rule r = PT_RULE(L("hello"));
    pt_grammar g = { r };
    
    pt_match_result res = pt_match(g, "hello", &opts);
    REQUIRE(res.matched == PT_NO_STACK_MEM);
}
