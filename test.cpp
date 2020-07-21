#include <stdio.h>
#include <stdlib.h>
#include "json-parser.h"
using namespace::std;

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

static void testNull() {
    Parser parser{ "null" };
    JsonToken val = parser.parse();
    EXPECT_EQ_INT(JsonType::kNull, val.type());
}

static void testTrue() {
    Parser parser{ "true" };
    JsonToken val = parser.parse();
    EXPECT_EQ_INT(JsonType::kBool, val.type());
}

static void testFalse() {
    Parser parser{ "true" };
    JsonToken val = parser.parse();
    EXPECT_EQ_INT(JsonType::kBool,val.type());
}


#define EXPECT_EQ_DOUBLE(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%.17g")

inline void testNumber(double expect, const char* json)
{
    Parser parser{ json };
    JsonToken macro_val = parser.parse();
    EXPECT_EQ_INT(JsonType::kNumber, macro_val.type());
    EXPECT_EQ_DOUBLE(expect, macro_val.toNumber());
}

static void testNumber() {
    testNumber(0.0, "0");
    testNumber(0.0, "-0");
    testNumber(0.0, "-0.0");
    testNumber(1.0, "1");
    testNumber(-1.0, "-1");
    testNumber(1.5, "1.5");
    testNumber(-1.5, "-1.5");
    testNumber(3.1416, "3.1416");
    testNumber(1E10, "1E10");
    testNumber(1e10, "1e10");
    testNumber(1E+10, "1E+10");
    testNumber(1E-10, "1E-10");
    testNumber(-1E10, "-1E10");
    testNumber(-1e10, "-1e10");
    testNumber(-1E+10, "-1E+10");
    testNumber(-1E-10, "-1E-10");
    testNumber(1.234E+10, "1.234E+10");
    testNumber(1.234E-10, "1.234E-10");
    testNumber(0.0, "1e-10000000000000"); /* underflow */

    testNumber(1.0000000000000002, "1.0000000000000002");             /* the smallest number > 1 */
    testNumber(4.9406564584124654e-324, "4.9406564584124654e-324");   /* minimum denormal */
    testNumber(-4.9406564584124654e-324,  "-4.9406564584124654e-324");
    testNumber(2.2250738585072009e-308,  "2.2250738585072009e-308");  /* Max subnormal double */
    testNumber(-2.2250738585072009e-308,  "-2.2250738585072009e-308");
    testNumber(2.2250738585072014e-308,  "2.2250738585072014e-308");  /* Min normal positive double */
    testNumber(-2.2250738585072014e-308,  "-2.2250738585072014e-308");
    testNumber(1.7976931348623157e+308, "1.7976931348623157e+308");   /* Max double */
    testNumber(-1.7976931348623157e+308,  "-1.7976931348623157e+308");
}


#define EXPECT_EQ_STRING(expect, actual, alength) \
    EXPECT_EQ_BASE(sizeof(expect) - 1 == alength && memcmp(expect, actual, alength) == 0, expect, actual, "%s")

#define TEST_STRING(expect, json)\
    do {\
        Parser parser{ json };\
        JsonToken val = parser.parse();\
        EXPECT_EQ_INT(JsonType::kString, val.type());\
        EXPECT_EQ_STRING(expect, val.toString().c_str(), val.toString().length());\
    } while(0)

static void testString() {
    TEST_STRING("", "\"\"");
    TEST_STRING("Hello", "\"Hello\"");
    TEST_STRING("Hello\nWorld", "\"Hello\\nWorld\"");
    TEST_STRING("\" \\ / \b \f \n \r \t", "\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"");
    TEST_STRING("Hello\0World", "\"Hello\\u0000World\"");
    TEST_STRING("\x24", "\"\\u0024\"");         /* Dollar sign U+0024 */
    TEST_STRING("\xC2\xA2", "\"\\u00A2\"");     /* Cents sign U+00A2 */
    TEST_STRING("\xE2\x82\xAC", "\"\\u20AC\""); /* Euro sign U+20AC */
    TEST_STRING("\xF0\x9D\x84\x9E", "\"\\uD834\\uDD1E\"");  /* G clef sign U+1D11E */
    TEST_STRING("\xF0\x9D\x84\x9E", "\"\\ud834\\udd1e\"");  /* G clef sign U+1D11E */
}


#if defined(_MSC_VER)
#define EXPECT_EQ_SIZE_T(expect, actual) EXPECT_EQ_BASE((expect) == (actual), (size_t)expect, (size_t)actual, "%Iu")
#else
#define EXPECT_EQ_SIZE_T(expect, actual) EXPECT_EQ_BASE((expect) == (actual), (size_t)expect, (size_t)actual, "%zu")
#endif

static void testArray() {
    Parser parser{ "[ ]" };
    JsonToken val = parser.parse();

    EXPECT_EQ_INT(JsonType::kArray, val.type());
    EXPECT_EQ_SIZE_T(0, val.size());

    parser.setContext("[ null , false , true , 123 , \"abc\" ]");
    val = parser.parse();
    EXPECT_EQ_INT(JsonType::kArray, val.type());
    EXPECT_EQ_SIZE_T(5, val.size());
    EXPECT_EQ_INT(JsonType::kNull, val[0].type());
    EXPECT_EQ_INT(JsonType::kBool, val[1].type());
    EXPECT_EQ_INT(JsonType::kBool, val[2].type());
    EXPECT_EQ_INT(JsonType::kNumber, val[3].type());
    EXPECT_EQ_INT(JsonType::kString, val[4].type());
    EXPECT_EQ_DOUBLE(123.0, val[3].toNumber());
    EXPECT_EQ_STRING("abc", val[4].toString().c_str(), val[4].toString().length());

    parser.setContext("[ [ ] , [ 0 ] , [ 0 , 1 ] , [ 0 , 1 , 2 ] ]");
    val = parser.parse();
    EXPECT_EQ_INT(JsonType::kArray, val.type());
    EXPECT_EQ_SIZE_T(4, val.toArray().size());
    for (int i = 0; i < 4; i++) {
        JsonToken arr_token = val.toArray()[i];
        EXPECT_EQ_INT(JsonType::kArray, arr_token.type());
        EXPECT_EQ_SIZE_T(i, arr_token.toArray().size());
        for (int j = 0; j < i; j++) {
            JsonToken num_token = arr_token.toArray()[j];
            EXPECT_EQ_INT(JsonType::kNumber, num_token.type());
            EXPECT_EQ_DOUBLE((double)j, num_token.toNumber());
        }
    };
}

static void testObject() {
    Parser parser{ "{ }" }; 
    JsonToken val = parser.parse();

    EXPECT_EQ_INT(JsonType::kObject, val.type());
    EXPECT_EQ_SIZE_T(0, val.toObject().size());

    parser.setContext(
        " { "
        "\"n\" : null , "
        "\"f\" : false , "
        "\"t\" : true , "
        "\"i\" : 123 , "
        "\"s\" : \"abc\", "
        "\"a\" : [ 1, 2, 3 ],"
        "\"o\" : { \"1\" : 1, \"2\" : 2, \"3\" : 3 }"
        " } "
    );
    val = parser.parse();
    
    EXPECT_EQ_INT(JsonType::kObject, val.type());
    EXPECT_EQ_SIZE_T(7, val.toObject().size());
    EXPECT_EQ_INT(JsonType::kNull, val["n"].type());
    EXPECT_EQ_INT(JsonType::kBool, val["f"].type());
    EXPECT_EQ_INT(JsonType::kBool, val["t"].type());
    EXPECT_EQ_INT(JsonType::kNumber, val["i"].type());
    EXPECT_EQ_DOUBLE(123.0, val["i"].toNumber());
    EXPECT_EQ_INT(JsonType::kString, val["s"].type());
    EXPECT_EQ_STRING("abc", val["s"].toString().c_str(), val["s"].toString().length());
    EXPECT_EQ_INT(JsonType::kArray, val["a"].type());
    EXPECT_EQ_SIZE_T(3, val["a"].size());
    for(int i = 0; i < 3; i++){
        EXPECT_EQ_INT(JsonType::kNumber, val["a"][i].type());
        EXPECT_EQ_DOUBLE(i + 1.0, val["a"][i].toNumber());
    }
    EXPECT_EQ_INT(JsonType::kObject, val["o"].type());
    string tmpKey = "";
    char start = '1';
    for (int i = 0; i < 3; i++){
        tmpKey = "";
        tmpKey += (start + i);
        EXPECT_EQ_INT(JsonType::kNumber, val["o"][tmpKey].type());
        EXPECT_EQ_DOUBLE(i + 1.0, val["o"][tmpKey].toNumber());
    }
}
static void testParse() {
    testNull();
    testFalse();
    testTrue();
    testNumber();
    testString();
    testArray();
    testObject();
}

int main() {
    testParse();
    printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
    return main_ret;
}
