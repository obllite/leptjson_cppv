#include <stdio.h>
#include <stdlib.h>
#include "leptjson.h"
#include <fstream>
using namespace::std;

static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;

// 原计划用template inline function取代原来的宏，但非完全特化模板函数用inline修饰没有意义,悲
// 同时改写成如下形式额外增加了进行强制转换的额外开销
void expect_eq_int(const int& expect_type,const int& actual_type)
{
    test_count++;
    if (expect_type == actual_type)
        test_pass++;
    else
    {
        std::ofstream outfile;
        outfile.open("error.txt", ios::app);
        main_ret = 1;
    }
}
static void test_parse_null() {
    lept_value* v = new lept_value{ lept_type::LEPT_FALSE };
    lept_context context{ "null" };
    expect_eq_int((int)(LEPT_PARSE_OK), v->lept_parse(context.lept_get_context()));
    expect_eq_int((int)lept_type::LEPT_NULL, (int)v->lept_get_type());
}

static void test_parse_expect_value() {
    lept_value* v = new lept_value{ lept_type::LEPT_FALSE };
    lept_context* context = new lept_context{ "" };
    expect_eq_int((int)LEPT_PARSE_EXPECT_VALUE, v->lept_parse(context->lept_get_context()));
    expect_eq_int((int)lept_type::LEPT_NULL, (int)v->lept_get_type());

    context->lept_set_context(" ");
    v->lept_set_type(lept_type::LEPT_FALSE);
    expect_eq_int((int)LEPT_PARSE_EXPECT_VALUE, v->lept_parse(context->lept_get_context()));
    expect_eq_int((int)lept_type::LEPT_NULL, (int)v->lept_get_type());
}

static void test_parse_invalid_value() {
    lept_value* v = new lept_value{ lept_type::LEPT_FALSE };
    lept_context* context = new lept_context{ "nul" };
    expect_eq_int((int)LEPT_PARSE_INVALID_VALUE, v->lept_parse(context->lept_get_context()));
    expect_eq_int((int)lept_type::LEPT_NULL, (int)v->lept_get_type());

    context->lept_set_context("?");
    v->lept_set_type(lept_type::LEPT_FALSE);
    expect_eq_int((int)LEPT_PARSE_INVALID_VALUE, v->lept_parse(context->lept_get_context()));
    expect_eq_int((int)lept_type::LEPT_NULL, (int)v->lept_get_type());
}

static void test_parse_root_not_singular() {
    lept_value* v = new lept_value{ lept_type::LEPT_FALSE };
    lept_context* context = new lept_context{ "null x" };
    expect_eq_int((int)LEPT_PARSE_ROOT_NOT_SINGULAR, v->lept_parse(context->lept_get_context()));
    expect_eq_int((int)lept_type::LEPT_NULL, (int)v->lept_get_type());
}

static void test_parse_true() {
    lept_value* v = new lept_value{ lept_type::LEPT_FALSE };
    lept_context* context = new lept_context{ "true" };    
    expect_eq_int((int)LEPT_PARSE_OK, v->lept_parse(context->lept_get_context()));
    expect_eq_int((int)lept_type::LEPT_TRUE, (int)v->lept_get_type());
}

static void test_parse_false() {
    lept_value* v = new lept_value{ lept_type::LEPT_FALSE };
    lept_context* context = new lept_context{ "false" };
    expect_eq_int((int)LEPT_PARSE_OK, v->lept_parse(context->lept_get_context()));
    expect_eq_int((int)lept_type::LEPT_FALSE, (int)v->lept_get_type());
}

static void test_parse() {
    test_parse_null();
    test_parse_expect_value();
    test_parse_invalid_value();
    test_parse_false();
    test_parse_true();
    test_parse_root_not_singular();
}

int main() {
    test_parse();
    printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
    return main_ret;
}