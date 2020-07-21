#pragma once
#include"my-exception.h"
#include"json-token.h"
#include<utility>
using namespace::std;

class JsonValue
{
public:
	virtual ~JsonValue() = default;

	virtual JsonType type() const = 0;

	virtual bool toBool() const {
		throw myException("This value is NOT a boolean");
	}
	virtual double toNumber() const {
		throw myException("This value is NOT a double");
	}
	virtual const string& toString() const {
		throw myException("This value is NOT a string");
	}
	virtual const JsonToken::array_t& toArray() const {
		throw myException("This value is NOT a array");
	}
	virtual const JsonToken::object_t& toObject() const {
		throw myException("This value is NOT a object");
	}

	virtual JsonToken& operator[](size_t) {
		throw myException("This value is NOT a array");
	}
	virtual const JsonToken& operator[](size_t) const {
		throw myException("This value is NOT a array");
	}

	virtual JsonToken& operator[](const string&) {
		throw myException("This value is NOT a object");
	}
	virtual const JsonToken& operator[](const string&) const {
		throw myException("This value is NOT a object");
	}

	virtual size_t size() const {
		return 0;
	}
};

template <typename T, JsonType U>
class Value : public JsonValue {
public:
	Value(const T& val) : val_(val) {}
	Value(T&& val) : val_(val) {}
	JsonType type() const final {
		return U;
	}

protected:
	T val_;
};

class JsonNull final : public Value<nullptr_t, JsonType::kNull>
{
public:
	explicit JsonNull(nullptr_t) : Value(nullptr) {}
};

class JsonBool final : public Value<bool, JsonType::kBool>
{
public:
	explicit JsonBool(bool val) : Value(val) {}
	bool toBool() const override {
		return val_;
	}
};

class JsonNumber final : public Value<double, JsonType::kNumber>
{
public:
	explicit JsonNumber(double val) : Value(val) {}
	double toNumber() const override {
		return val_;
	}
};

class JsonString final : public Value<string, JsonType::kString>
{
public:
	explicit JsonString(const string& val) : Value(val) {}
	explicit JsonString(string&& val) : Value(move(val)) {}
	const string& toString() const override {
		return val_;
	}
};

class JsonArray final : public Value<JsonToken::array_t, JsonType::kArray>
{
public:
	explicit JsonArray(const JsonToken::array_t& val) : Value(val) {}
	explicit JsonArray(JsonToken::array_t&& val) noexcept: Value(move(val)) {}

	const JsonToken::array_t& toArray() const override {
		return val_;
	}

	const JsonToken& operator[](size_t i)const override {
		return val_[i];
	}

	JsonToken& operator[](size_t i) override {
		return val_[i];
	}

	size_t size() const override {
		return val_.size();
	}
};

class JsonObject final : public Value<JsonToken::object_t, JsonType::kObject> {
public:
	explicit JsonObject(const JsonToken::object_t& val) : Value(val) {}
	explicit JsonObject(JsonToken::object_t&& val) : Value(move(val)) {}

	const JsonToken::object_t& toObject() const override {
		return val_;
	}

	const JsonToken& operator[](const string& i) const override {
		return val_.at(i);
	}

	JsonToken& operator[](const string& i) override {
		return val_.at(i);
	}

	size_t size() const override {
		return val_.size();
	}
};
