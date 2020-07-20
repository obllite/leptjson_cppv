#ifndef LEPTJSON_VALUE_H__
#define LEPTJSON_VAlUE_H__
#include <iostream>
#include <assert.h>
using namespace::std;

enum class lept_type
{
	LEPT_NULL,
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
	lept_value(lept_type type_, double num_):type(type_), num(num_) {};
	~lept_value() = default;

	lept_type lept_get_type() { return type; };
	double lept_get_num() { assert(type == lept_type::LEPT_NUMBER); return num; }
	void lept_set_type(const lept_type type_) { this->type = type_; }
	void lept_set_num(const double num_) { this->num = num_; }
private:
	lept_type type;
	double num;
};

enum 
{
	LEPT_PARSE_OK = 0,
	LEPT_PARSE_EXPECT_VALUE,
	LEPT_PARSE_INVALID_VALUE,
	LEPT_PARSE_ROOT_NOT_SINGULAR,
	LEPT_PARSE_NUMBER_TOO_BIG
};

#endif
