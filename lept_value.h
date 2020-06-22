#ifndef LEPTJSON_VALUE_H__
#define LEPTJSON_VAlUE_H__
#include <iostream>
#include <assert.h>
using namespace::std;

enum class lept_type
{
	LEPT_NULL = 0,
	LEPT_FALSE,
	LEPT_TRUE,
	LEPT_NUMBER,
	LEPT_STRING,
	LEPT_ARRAY,
	LEPT_OBJECT
};

class lept_value
{
public:

	lept_value() = default;
	lept_value(lept_type type_):type(type_) {};
	~lept_value() = default;

	lept_type lept_get_type() { return type; };
	void lept_set_type(const lept_type type_) { this->type = type_; }
private:
	lept_type type;
};

enum 
{
	LEPT_PARSE_OK = 0,
	LEPT_PARSE_EXPECT_VALUE,
	LEPT_PARSE_INVALID_VALUE,
	LEPT_PARSE_ROOT_NOT_SINGULAR
};

#endif
