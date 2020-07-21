#include "json-parser.h"
#include<cassert>
#include<cstring>
#include <cstdlib>
#include <stdexcept>
using namespace::std;
// Constexpr functions and constexpr constructors are implicitly inline
constexpr bool is_digit(const char& ch) { return (ch >= '0') && (ch <= '9'); }
constexpr bool is_digit_1to9(const char& ch) { return (ch >= '1') && (ch <= '9'); }

JsonToken Parser::parse()
{
	this->parseWhitespace();
	JsonToken token = this->parseValue();
	this->parseWhitespace();

	if (*(this->parsed_context_))
		parseErr("ROOT NOT SINGULAR!");

	return token;
}

void Parser::parseWhitespace()
{
	const char* p = this->parsed_context_;
	while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
		p++;
	this->parsed_context_ = p;
}

JsonToken Parser::parseValue()
{
	switch (*(this->parsed_context_))
	{
	case 'f': return this->parseLiteral("false");
	case 'n': return this->parseLiteral("null");
	case 't': return this->parseLiteral("true");
	case '\"': return this->parseString();
	case '[': return this->parseArray();
	case '{': return this->parseObject();
	case '\0': parseErr("EXEPECT VALUE!");
	default: return this->parseNumber();
	}
}

JsonToken Parser::parseLiteral(const string& parsed_literal) {
	if (strncmp(this->parsed_context_, parsed_literal.c_str(), parsed_literal.size())) parseErr("INVALID VALUE");
	this->parsed_context_ += parsed_literal.size();
	switch (parsed_literal[0])
	{
	case 't':return JsonToken(true);
	case 'f':return JsonToken(false);
	default: return JsonToken(nullptr);
	}
}
// Abandoned and replaced
JsonToken Parser::parseFalse()
{
	assert(*(this->parsed_context_) == 'f');
	this->parsed_context_++;
	if (this->parsed_context_[0] != 'a' || this->parsed_context_[1] != 'l' || this->parsed_context_[2] != 's' || this->parsed_context_[3] != 'e')
		parseErr("INVALID VALUE!");

	this->parsed_context_ += 4;
	return JsonToken(false);
}
// Abandoned and replaced
JsonToken Parser::parseTrue()
{
	assert(*(this->parsed_context_) == 't');
	this->parsed_context_++;
	if (this->parsed_context_[0] != 'r' || this->parsed_context_[1] != 'u' || this->parsed_context_[2] != 'e')
		parseErr("INVALID VALUE!");
	this->parsed_context_ += 3;
	return JsonToken(true);
}
// Abandoned and replaced
JsonToken Parser::parseNull()
{
	assert(*(this->parsed_context_) == 'n');
	this->parsed_context_++;
	if (this->parsed_context_[0] != 'u' || this->parsed_context_[1] != 'l' || this->parsed_context_[2] != 'l')
		parseErr("INVALID VALUE!");
	this->parsed_context_ += 3;
	return JsonToken(nullptr);
}

JsonToken Parser::parseNumber()
{
	const char* p = this->parsed_context_;
	if (*p == '-') p++;
	if (*p == '0') p++;
	// integer
	else
	{
		if (!is_digit_1to9(*p)) parseErr("INVALID VALUE!");
		while (is_digit(*++p));
	}
	// frac
	if (*p == '.')
	{
		p++;
		if (!is_digit(*p)) parseErr("INVALID VALUE!");
		while (is_digit(*++p));
	}
	// exp
	if (toupper(*p) == 'E')
	{
		p++;
		if (*p == '+' || *p == '-') p++;
		if (!is_digit(*p)) parseErr("INVALID VALUE!");
		while (is_digit(*++p));
	}

	errno = 0;

	JsonToken value = JsonToken(strtod(this->parsed_context_, NULL));
	if (errno == ERANGE && (value.toNumber() == HUGE_VAL || value.toNumber() == -HUGE_VAL))
		parseErr("NUMBER TOO BIG!");
	this->parsed_context_ = p;
	return value;
}

string Parser::parseRawString() {
	string str;
	while (true)
	{
		switch (*++this->parsed_context_) {
		case '\"': ++this->parsed_context_;  return str;
		case '\0': parseErr("MISS QUOTATION MARK!");
		default:
			if (static_cast<unsigned char>(*parsed_context_) < 32) parseErr("INVALID STRING CHAR!");
			str += *parsed_context_;
			break;
		case '\\':
			switch (*++parsed_context_) // Comparison between append, += and push_back
			{
			case '\"': str += '\"'; break;
			case '\\': str += '\\'; break;
			case '/': str += '/'; break; // In Json, '\/' is equal to '/'
			case 'b': str += '\b'; break;
			case 'f': str += '\f'; break;
			case 'n': str += '\n'; break;
			case 't': str += '\t'; break;
			case 'r': str += '\r'; break;
			case 'u': {
				unsigned u1 = parse4hex();
				if (u1 >= 0xd800 && u1 <= 0xdbff) {  // high surrogate
					if (*++parsed_context_ != '\\') parseErr("INVALID UNICODE SURROGATE");
					if (*++parsed_context_ != 'u') parseErr("INVALID UNICODE SURROGATE");
					unsigned u2 = parse4hex();  // low surrogate
					if (u2 < 0xdc00 || u2 > 0xdfff) parseErr("INVALID UNICODE SURROGATE");
					u1 = (((u1 - 0xd800) << 10) | (u2 - 0xdc00)) + 0x10000;
				}
				str += encodeUTF8(u1);
			}
					break;
			default:
				parseErr("INVALID STRING ESCAPE!");
			}
			break;
		}
	}
}

unsigned Parser::parse4hex() {
	unsigned u = 0;
	for (int i = 0; i != 4; ++i) {
		unsigned ch = static_cast<unsigned>(toupper(*++parsed_context_));
		u <<= 4;  // <<= 4 equal to *= 16
		if (is_digit(ch))
			u |= (ch - '0');  // |= equal to +=
		else if (ch >= 'A' && ch <= 'F')
			u |= ch - 'A' + 10;
		else
			parseErr("INVALID UNICODE HEX");
	}
	return u;
}

string Parser::encodeUTF8(unsigned u) {
	string utf8;
	if (u <= 0x7f)  // 0xxxxxxx
		utf8 += static_cast<char>(u & 0xff);
	else if (u <= 0x7FF) { // 110xxxxx 10xxxxxx
		utf8 += (static_cast<char>(0xc0 | ((u >> 6) & 0xff)));
		utf8 += static_cast<char>(0x80 | (u & 0x3f));
	}
	else if (u <= 0xFFFF) { // 1110xxxx 10xxxxxx 10xxxxxx
		utf8 += static_cast<char>(0xe0 | ((u >> 12) & 0xff));
		utf8 += static_cast<char>(0x80 | ((u >> 6) & 0x3f));
		utf8 += static_cast<char>(0x80 | (u & 0x3f));
	}
	else if (u <= 0x10FFFF) { // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
		utf8 += static_cast<char>(0xf0 | ((u >> 18) & 0xff));
		utf8 += static_cast<char>(0x80 | ((u >> 12) & 0x3f));
		utf8 += static_cast<char>(0x80 | ((u >> 6) & 0x3f));
		utf8 += static_cast<char>(0x80 | (u & 0x3f));
	}
	else
		parseErr("INVALID UTF-8 CODE");

	return utf8;
}

JsonToken Parser::parseArray() {
	JsonToken::array_t arr;
	++this->parsed_context_; // For skipping '['
	parseWhitespace();
	if (*this->parsed_context_ == ']') {
		this->parsed_context_++;
		return JsonToken(arr);
	}

	while (true)
	{
		this->parseWhitespace();
		arr.push_back(parseValue());
		this->parseWhitespace();
		if (*this->parsed_context_ == ',')
			++this->parsed_context_;
		else if (*this->parsed_context_ == ']') {
			++this->parsed_context_;
			return JsonToken(arr);
		}
		else
			parseErr("MISS COMMA OR SQUARE BACKET!");
	}
}

JsonToken Parser::parseObject() {
	JsonToken::object_t obj; 
	++this->parsed_context_;  // For skipping '{'
	this->parseWhitespace();
	if (*this->parsed_context_ == '}') {
		this->parsed_context_++;
		return JsonToken{ obj };
	}

	while (true)
	{
		this->parseWhitespace();
		if (*this->parsed_context_ != '"')
			parseErr("MISS KEY!");

		string key = this->parseRawString();

		this->parseWhitespace();
		if (*this->parsed_context_++ != ':')
			parseErr("MISS COLON!");
		this->parseWhitespace();

		JsonToken val = this->parseValue();
		obj.insert({ key, val });

		this->parseWhitespace();

		if (*this->parsed_context_ == ',')
			++this->parsed_context_;
		else if (*this->parsed_context_ == '}') {
			this->parsed_context_++;
			return JsonToken{ obj };
		}
		else
			parseErr("MISS COMMA OR MISS CURLY BACKET!");
	}
}
