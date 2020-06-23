#ifndef LEPTJSON_CONTEXT_H__
#define LEPTJSON_CONTEXT_H__
#include <iostream>
#include <assert.h>
#include "lept_value.h"
using namespace::std;

class lept_context
{
public:
	lept_context() = default;
	lept_context(const char* json_) : json(json_) { };
	lept_context& operator=(const lept_context& context) { if (this != &context)json = context.json; }

	~lept_context() = default;

	void lept_set_context(const char* json_) { this->json = json_; }
	const char* lept_get_context(void) { return this->json; }

	void lept_parse_whitespace();
	int lept_parse_false(lept_value& v);
	int lept_parse_true(lept_value& v);
	int lept_parse_null(lept_value& v);
	int lept_parse_value(lept_value& v);
	int lept_parse_number(lept_value& v);
	int lept_parse(lept_value& v);

private:
	const char* json;
};

#endif
