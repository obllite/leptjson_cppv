#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "leptjson.h"

static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;

#define EXPECT_EQ_BASE(equality, expect, actual, format) \
    do {\
        test_count++;\
        if (equality)\
            test_pass++;\
        else {\
            fprintf(stderr, "%s:%d: expect: " format " actual: " format "\n", __FILE__, __LINE__, expect, actual);\
            main_ret = 1;\
        }\
    } while(0)

#define EXPECT_EQ_INT(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%d")

static void test_parse_null() {
    lept_value* v = new lept_value{ lept_type::LEPT_FALSE };
    lept_context context{ "null" };

    EXPECT_EQ_INT(LEPT_PARSE_OK, v->lept_parse(context.lept_get_context()));
    EXPECT_EQ_INT(lept_type::LEPT_NULL, v->lept_get_type());

}

static void test_parse_expect_value() {
    lept_value* v = new lept_value{ lept_type::LEPT_FALSE };
    lept_context* context = new lept_context{ "" };

    EXPECT_EQ_INT(LEPT_PARSE_EXPECT_VALUE, v->lept_parse(context->lept_get_context()));
    EXPECT_EQ_INT(lept_type::LEPT_NULL, v->lept_get_type());

    context->lept_set_context(" ");
    v->lept_set_type(lept_type::LEPT_FALSE);
    EXPECT_EQ_INT(LEPT_PARSE_EXPECT_VALUE, v->lept_parse(context->lept_get_context()));
    EXPECT_EQ_INT(lept_type::LEPT_NULL, v->lept_get_type());
}

static void test_parse_invalid_value() {
    lept_value* v = new lept_value{lept_type::LEPT_FALSE};
    lept_context* context = new lept_context{ "nul" };

    EXPECT_EQ_INT(LEPT_PARSE_INVALID_VALUE, v->lept_parse(context->lept_get_context()));
    EXPECT_EQ_INT(lept_type::LEPT_NULL, v->lept_get_type());

    context->lept_set_context("?");
    v->lept_set_type(lept_type::LEPT_FALSE);
    EXPECT_EQ_INT(LEPT_PARSE_INVALID_VALUE, v->lept_parse(context->lept_get_context()));
    EXPECT_EQ_INT(lept_type::LEPT_NULL, v->lept_get_type());
}

static void test_parse_root_not_singular() {
    lept_value* v = new lept_value{lept_type::LEPT_FALSE};
    lept_context* context = new lept_context{ "null x" };
    v->lept_set_type(lept_type::LEPT_FALSE);
    EXPECT_EQ_INT(LEPT_PARSE_ROOT_NOT_SINGULAR, v->lept_parse(context->lept_get_context()));
    EXPECT_EQ_INT(lept_type::LEPT_NULL, v->lept_get_type());
}

static void test_parse() {
    test_parse_null();
    test_parse_expect_value();
    test_parse_invalid_value();
    test_parse_root_not_singular();
}

int main() {
    test_parse();
    printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
    return main_ret;
}