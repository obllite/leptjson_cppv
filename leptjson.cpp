#include "leptjson.h"


void lept_context::lept_parse_whitespace()
{
	const char* p = this->json;
	while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')		
		p++;
	this->json = p;
}

int lept_context::lept_parse_false(lept_value& v)
{
	assert(*(this->json) == 'f');
	this->json++;
	if (this->json[0] != 'a' || this->json[1] != 'l' || this->json[2] != 's' || this->json[3] != 'e')
		return LEPT_PARSE_INVALID_VALUE;
	this->json += 4;
	v.lept_set_type(lept_type::LEPT_FALSE);
	return LEPT_PARSE_OK;
}

int lept_context::lept_parse_true(lept_value& v)
{
	assert(*(this->json) == 't');
	this->json++;
	if (this->json[0] != 'r' || this->json[1] != 'u' || this->json[2] != 'e')
		return LEPT_PARSE_INVALID_VALUE;
	this->json += 3;
	v.lept_set_type(lept_type::LEPT_TRUE);
	return LEPT_PARSE_OK;
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
		case 'f': return this->lept_parse_false(v);
		case 'n': return this->lept_parse_null(v);
		case 't': return this->lept_parse_true(v);
		case '\0': return LEPT_PARSE_EXPECT_VALUE;
		default: return LEPT_PARSE_INVALID_VALUE;
	}
}

int lept_value::lept_parse(const char* json)
{
	lept_context* c = new lept_context(json);
	int ret;
	this->type = lept_type::LEPT_NULL;

	c->lept_parse_whitespace();
	if ((ret = c->lept_parse_value(*this)) == LEPT_PARSE_OK)
	{
		c->lept_parse_value(*this);
		if (*c->lept_get_context() != '\0')
			ret = LEPT_PARSE_ROOT_NOT_SINGULAR;
	}
	return ret;
}
