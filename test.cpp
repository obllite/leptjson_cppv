#include <stdio.h>
#include <stdlib.h>
#include "lept_context.h"
using namespace::std;

static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;

// 宏函数，永允滴神。只用inline没有通用性，只用template没有效率，两者都用的inline template只在完全特化模板的情况下
// 编译器才会接受inline建议，等于没用template。
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
#define EXPECT_EQ_DOUBLE(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%.17g")

#define TEST_NUMBER(expect, json)\
    do {\
        lept_value macro_v;\
        lept_context context{json};\
        EXPECT_EQ_INT(LEPT_PARSE_OK, context.lept_parse(macro_v));\
        EXPECT_EQ_INT(lept_type::LEPT_NUMBER, macro_v.lept_get_type());\
        EXPECT_EQ_DOUBLE(expect ,macro_v.lept_get_num());\
    } while(0)

#define TEST_ERROR(error,  json)\
    do {\
        lept_value macro_v{lept_type::LEPT_FALSE, 0};\
        lept_context context{json};\
        EXPECT_EQ_INT(error, context.lept_parse(macro_v));\
        EXPECT_EQ_INT(lept_type::LEPT_NULL, macro_v.lept_get_type());\
    } while(0)

static void test_parse_null() {
    lept_value value{ lept_type::LEPT_FALSE, 0 };
    lept_context context{ "null" };
    EXPECT_EQ_INT((LEPT_PARSE_OK), context.lept_parse(value));
    EXPECT_EQ_INT(lept_type::LEPT_NULL, value.lept_get_type());
}

static void test_parse_true() {
    lept_value value{ lept_type::LEPT_FALSE, 0 };
    lept_context context{ "true" };
    EXPECT_EQ_INT(LEPT_PARSE_OK, context.lept_parse(value));
    EXPECT_EQ_INT(lept_type::LEPT_TRUE, value.lept_get_type());
}

static void test_parse_false() {
    lept_value value{ lept_type::LEPT_FALSE, 0 };
    lept_context context{ "false" };
    EXPECT_EQ_INT(LEPT_PARSE_OK, context.lept_parse(value));
    EXPECT_EQ_INT(lept_type::LEPT_FALSE, value.lept_get_type());
}

static void test_parse_number() {
    TEST_NUMBER(0.0, "0");
    TEST_NUMBER(0.0, "-0");
    TEST_NUMBER(0.0, "-0.0");
    TEST_NUMBER(1.0, "1");
    TEST_NUMBER(-1.0, "-1");
    TEST_NUMBER(1.5, "1.5");
    TEST_NUMBER(-1.5, "-1.5");
    TEST_NUMBER(3.1416, "3.1416");
    TEST_NUMBER(1E10, "1E10");
    TEST_NUMBER(1e10, "1e10");
    TEST_NUMBER(1E+10, "1E+10");
    TEST_NUMBER(1E-10, "1E-10");
    TEST_NUMBER(-1E10, "-1E10");
    TEST_NUMBER(-1e10, "-1e10");
    TEST_NUMBER(-1E+10, "-1E+10");
    TEST_NUMBER(-1E-10, "-1E-10");
    TEST_NUMBER(1.234E+10, "1.234E+10");
    TEST_NUMBER(1.234E-10, "1.234E-10");
    TEST_NUMBER(0.0, "1e-10000000000000"); /* 下溢 */

    TEST_NUMBER(1.0000000000000002, "1.0000000000000002");            /* 所有大于0的数中最小者 */
    TEST_NUMBER(4.9406564584124654e-324, "4.9406564584124654e-324");  /* double类型最小非规格化浮点数 */
    TEST_NUMBER(-4.9406564584124654e-324,  "-4.9406564584124654e-324");
    TEST_NUMBER(2.2250738585072009e-308,  "2.2250738585072009e-308");  /* double类型的最大非规格化浮点数 */
    TEST_NUMBER(-2.2250738585072009e-308,  "-2.2250738585072009e-308");
    TEST_NUMBER(2.2250738585072014e-308,  "2.2250738585072014e-308");  /* 大于0的最小正规double类型 */
    TEST_NUMBER(-2.2250738585072014e-308,  "-2.2250738585072014e-308");
    TEST_NUMBER(1.7976931348623157e+308, "1.7976931348623157e+308");  /* double类型最大值 */
    TEST_NUMBER(-1.7976931348623157e+308,  "-1.7976931348623157e+308");
}

static void test_parse_expect_value() {
    TEST_ERROR(LEPT_PARSE_EXPECT_VALUE, "");
    TEST_ERROR(LEPT_PARSE_EXPECT_VALUE, " ");
}

static void test_parse_invalid_value() {
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "nul");
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "?");

    /* 非法数字 */
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "+0");
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "+1");
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, ".123"); 
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "1."); 
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "INF");
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "inf");
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "NAN");
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "nan");
}

static void test_parse_root_not_singular() {
    TEST_ERROR(LEPT_PARSE_ROOT_NOT_SINGULAR, "null x");

    TEST_ERROR(LEPT_PARSE_ROOT_NOT_SINGULAR, "0123"); 
    TEST_ERROR(LEPT_PARSE_ROOT_NOT_SINGULAR, "0x0");
    TEST_ERROR(LEPT_PARSE_ROOT_NOT_SINGULAR, "0x123");
}

static void test_parse_number_too_big() {
    TEST_ERROR(LEPT_PARSE_NUMBER_TOO_BIG, "1e309");
    TEST_ERROR(LEPT_PARSE_NUMBER_TOO_BIG, "-1e309");
}

static void test_parse() {
    test_parse_null();
    test_parse_expect_value();
    test_parse_invalid_value();
    test_parse_false();
    test_parse_true();
    test_parse_root_not_singular();
    test_parse_number();
    test_parse_number_too_big();
}

int main() {
    test_parse();
    printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
    return main_ret;
}