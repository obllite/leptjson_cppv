#ifndef LEPTJSON_H__
#define LEPTJSON_H__
#include <string>
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
	lept_value(lept_type type_):type(type_) {};
	~lept_value() = default;

	lept_type lept_get_type() { return type; };
	void lept_set_type(const lept_type type_) { this->type = type_; }
	int lept_parse(const char* json);
private:
	lept_type type;
};

class lept_context
{
public:
	lept_context() = default;
	lept_context(const char* json_): json(json_){ };
	lept_context& operator=(const lept_context& context) { if (this != &context)json = context.json; }

	~lept_context() = default;

	void lept_set_context(const char* json_) { this->json = json_; }
	const char* lept_get_context(void) { return this->json; }

	void lept_parse_whitespace();
	int lept_parse_true(lept_value& v);
	int lept_parse_null(lept_value& v);
	int lept_parse_value(lept_value& v);
private:
	const char* json;
};

enum 
{
	LEPT_PARSE_OK = 0,
	LEPT_PARSE_EXPECT_VALUE,
	LEPT_PARSE_INVALID_VALUE,
	LEPT_PARSE_ROOT_NOT_SINGULAR
};

#endif
