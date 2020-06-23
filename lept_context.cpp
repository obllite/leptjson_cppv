#include "lept_context.h"

inline bool is_digit(const char& ch) { return (ch >= '0') && (ch <= '9'); }
inline bool is_digit_1to9(const char& ch) { return (ch >= '1') && (ch <= '9'); }

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

int lept_context::lept_parse_value(lept_value& value)
{
	switch (*(this->json))
	{
		case 'f': return this->lept_parse_false(value);
		case 'n': return this->lept_parse_null(value);
		case 't': return this->lept_parse_true(value);
		default: return this->lept_parse_number(value);
		case '\0': return LEPT_PARSE_EXPECT_VALUE;
	}
}

int lept_context::lept_parse_number(lept_value& value)
{
	const char* p = this->json;
	if (*p == '-') p++;
	if (*p == '0') p++;
	// 整数
	else 
	{
		if (!is_digit_1to9(*p)) return LEPT_PARSE_INVALID_VALUE;
		while (is_digit(*++p));
	}
	// 小数
	if (*p == '.') 
	{
		p++;
		if (!is_digit(*p)) return LEPT_PARSE_INVALID_VALUE;
		while (is_digit(*++p));
	}
	// exp
	if (toupper(*p) == 'E')
	{
		p++;
		if (*p == '+' || *p == '-') p++;
		if (!is_digit(*p)) return LEPT_PARSE_INVALID_VALUE;
		while (is_digit(*++p));
	}
	errno = 0;
	value.lept_set_num(strtod(this->json, NULL));
	value.lept_set_type(lept_type::LEPT_NUMBER);
	if (errno == ERANGE && (value.lept_get_num() == HUGE_VAL || value.lept_get_num() == -HUGE_VAL))
	{
		value.lept_set_type(lept_type::LEPT_NULL);
		return LEPT_PARSE_NUMBER_TOO_BIG;
	}
	this->json = p;
	return LEPT_PARSE_OK;
}

int lept_context::lept_parse(lept_value& value)
{
	int ret;
	assert(value.lept_get_type() != lept_type::LEPT_NULL);

	value.lept_set_type(lept_type::LEPT_NULL);

	this->lept_parse_whitespace();
	if ((ret = this->lept_parse_value(value)) == LEPT_PARSE_OK)
	{
		this->lept_parse_whitespace();
		if (*this->json != '\0')
		{
			value.lept_set_type(lept_type::LEPT_NULL);
			ret = LEPT_PARSE_ROOT_NOT_SINGULAR;
		}
	}
	return ret;
}
