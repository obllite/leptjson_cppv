#pragma once
#include "json-token.h"
#include"my-exception.h"
using namespace::std;

class Parser final
{
public:
	Parser() = default;
	Parser(const char* context) : parsed_context_(context) { };
	Parser& operator=(const Parser& context) { if (this != &context)parsed_context_ = context.parsed_context_; }

	~Parser() = default;

	void setContext(const char* context) { this->parsed_context_ = context; }
	const char* getContext(void) { return this->parsed_context_; }

	JsonToken parse();


private:
	void parseWhitespace();
	string parseRawString();
	unsigned parse4hex();
	string encodeUTF8(unsigned u);
	JsonToken parseValue();
	JsonToken parseFalse();
	JsonToken parseTrue();
	JsonToken parseNull();
	JsonToken parseNumber();
	JsonToken parseLiteral(const string& literal);
	JsonToken parseString() { return JsonToken(parseRawString()); }
	JsonToken parseArray();
	JsonToken parseObject();

	void parseErr(const string& err_msg) const {
		throw myException(err_msg + ": " + parsed_context_);
	}

	const char* parsed_context_;
};

