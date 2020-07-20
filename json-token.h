#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

using namespace::std;

class JsonValue;

// The representations of different kinds of JSON values
enum class JsonType
{
	kNull,
	kBool,
	kNumber,
	kString,
	kArray,
	kObject
};

class JsonToken final {
public:
	using array_t = vector<JsonToken>;
	using object_t = unordered_map<string, JsonToken>;

	explicit JsonToken(nullptr_t);										// null
	explicit JsonToken(bool);											// boolean
	explicit JsonToken(int val) : JsonToken(1.0 * val) {};				// number
	explicit JsonToken(double);											// number
	explicit JsonToken(const char* cstr) : JsonToken(string(cstr)) {};	// string
	explicit JsonToken(const string&);									// string
	explicit JsonToken(const array_t&);						// array
	explicit JsonToken(const object_t&);						// object

	JsonToken(const JsonToken&) noexcept;
	JsonToken(JsonToken&&) noexcept;
	~JsonToken() noexcept {};

	// Accesses the type of JSON value the current value instance is
	JsonType type() const noexcept;
	// Is the current value a null value?
	bool isNull() const noexcept;
	// Is the current value a boolean value?
	bool isBool() const noexcept;
	// Is the current value a number value?
	bool isNumber() const noexcept;
	// Is the current value a string value?
	bool isString() const noexcept;
	// Is the current value a array value?
	bool isArray() const noexcept;
	// Is the current value a object value?
	bool isObject() const noexcept;

	// Converts the JSON value to a C++ boolean, if and only if it is a boolean
	bool toBool() const;
	// Converts the JSON value to a C++ number(double), if and only if it is a double
	double toNumber() const;
	// Converts the JSON value to a C++ string, if and only if it is a string
	const std::string& toString() const;
	// Converts the JSON value to a json array, if and only if it is an array
	const array_t& toArray() const;
	// Converts the JSON value to a json object, if and only if it is an object
	const object_t& toObject() const;

	// Accesses a field of a JSON array
	JsonToken& operator[](size_t);
	// Accessed a field of a JSON array
	const JsonToken& operator[](std::size_t) const;
	// Accesses a field of a JSON object
	JsonToken& operator[](const std::string&);
	// Accesses a field of a JSON object
	const JsonToken& operator[](const std::string&) const;

	// Get the number of children of the value, 0 for all non-composites
	std::size_t size() const noexcept;


	// Serializes the JSON value to C++ string
	string serialize() const noexcept;
private:
	// The functions below are used by serialize()
	string serializeString() const;
	string serializeArray() const;
	string serializeObject() const;

	unique_ptr<JsonValue> value_;
};