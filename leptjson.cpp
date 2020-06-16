#include "leptjson.h"
#include <assert.h>
#include <iostream>

void lept_context::lept_parse_whitespace()
{
	auto p = this->json;
	while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')		
		++p;
	this->json = p;
}

int lept_context::lept_parse_null(lept_value& v)
{
	assert(*(this->json) == 'n');
	this->json++;
	if (this->json[0] != 'u' || this->json[1] != 'l' || this->json[2] != 'l') 
		return LEPT_PARSE_INVALID_VALUE;
	this->json += 3;
	v.lept_set_type(lept_type::LEPT_NULL);
	return LEPT_PARSE_OK;
}

int lept_context::lept_parse_value(lept_value& v)
{
	switch (*(this->json))
	{
		case 'n': return this->lept_parse_null(v);
		case '\0': return LEPT_PARSE_EXPECT_VALUE;
		default: return LEPT_PARSE_INVALID_VALUE;
	}
}

int lept_value::lept_parse(const char* json)
{
	lept_context* c = new lept_context(json);
	this->type = lept_type::LEPT_NULL;
	c->lept_parse_whitespace();
	return c->lept_parse_value(*this);
}
