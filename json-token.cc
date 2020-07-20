#include"json-token.h"
#include"json-parser.h"
#include"json-value.h"
#include<cassert>
using namespace::std;

JsonToken::JsonToken(nullptr_t) : value_(make_unique<JsonNull>(nullptr)) {}
JsonToken::JsonToken(bool val) : value_(make_unique<JsonBool>(val)) {}
JsonToken::JsonToken(double val) : value_(make_unique<JsonNumber>(val)) {}
JsonToken::JsonToken(const string& val) : value_(make_unique<JsonString>(val)) {}
JsonToken::JsonToken(const array_t& val) : value_(make_unique<JsonArray>(val)) {}
JsonToken::JsonToken(const object_t& val) : value_(make_unique<JsonObject>(val)) {}

JsonToken::JsonToken(const JsonToken& token) noexcept {
	switch (token.type()) {
	case JsonType::kNull: value_ = make_unique<JsonNull>(nullptr); break;
	case JsonType::kBool: value_ = make_unique<JsonBool>(token.toBool()); break;
	case JsonType::kNumber: value_ = make_unique<JsonNumber>(token.toNumber()); break;
	case JsonType::kString: value_ = make_unique<JsonString>(token.toString()); break;
	case JsonType::kArray: value_ = make_unique<JsonArray>(token.toArray()); break;
	case JsonType::kObject: value_ = make_unique<JsonObject>(token.toObject()); break;
	}
}

JsonToken::JsonToken(JsonToken&& rhs) noexcept : value_(std::move(rhs.value_)) { rhs.value_ = nullptr; }

// This function was called by ctor so keyword noexcept is neccessary.
JsonType JsonToken::type() const { return value_->type(); }

bool JsonToken::isNull() const { return type() == JsonType::kNull; }
bool JsonToken::isBool() const { return type() == JsonType::kBool; }
bool JsonToken::isNumber() const { return type() == JsonType::kNumber; }
bool JsonToken::isString() const { return type() == JsonType::kString; }
bool JsonToken::isArray() const { return type() == JsonType::kArray; }
bool JsonToken::isObject() const { return type() == JsonType::kObject; }

bool JsonToken::toBool() const { return value_->toBool(); }
double JsonToken::toNumber() const { return value_->toNumber(); }
const string& JsonToken::toString() const { return value_->toString(); }
const JsonToken::array_t& JsonToken::toArray() const { return value_->toArray(); }
const JsonToken::object_t& JsonToken::toObject() const { return value_->toObject(); }

string JsonToken::serialize() const noexcept {
	switch (value_->type())
	{
	case JsonType::kNull: return "null";
	case JsonType::kBool: return value_->toBool() ? "true" : "false";
	case JsonType::kNumber:
		char buf[32];
		snprintf(buf, sizeof(buf), "%.17g", value_->toNumber());
		return buf;
	case JsonType::kString: return serializeString();
	case JsonType::kArray: return serializeArray();
	case JsonType::kObject: return serializeObject();
	}
	assert(0);
}

string JsonToken::serializeString() const noexcept {
	string ret = "\"";
	for (auto e : value_->toString()) {
		switch (e) {
		case '\"': ret += "\\\""; break;
		case '\\': ret += "\\\\"; break;
		case '\b': ret += "\\b"; break;
		case '\f': ret += "\\f"; break;
		case '\n': ret += "\\n"; break;
		case '\r': ret += "\\r"; break;
		case '\t': ret += "\\t"; break;
		default:
			if (static_cast<unsigned char>(e) < 0x20) {
				char buf[7];
				sprintf(buf, "\\u%04X", e);
				ret += buf;
			}
			else
				ret += e;
		}
	}
	return ret + '"';
}

string JsonToken::serializeArray() const noexcept {
	string ret = "[ ";
	for (size_t i = 0; i < value_->size(); ++i) {
		if (i > 0) ret += ", ";
		ret += (*this)[i].serialize();
	}
	return ret + " ]";
}

string JsonToken::serializeObject() const noexcept {
	string ret = "{ ";
	bool first = 1;
	for (const pair<string, JsonToken>& p : value_->toObject()) {
		if (first)
			first = 0;
		else
			ret += ", ";

		ret += "\"" + p.first + "\"";
		ret += ": ";
		ret += p.second.serialize();
	}
	return ret + " }";
}

inline std::ostream& operator<<(ostream& os, const JsonToken& json) {
	return os << json.serialize();
}

JsonToken& JsonToken::operator[](size_t i) { return value_->operator[](i); }
const JsonToken& JsonToken::operator[](size_t i) const { return value_->operator[](i); }
JsonToken& JsonToken::operator[](const string& i) { return value_->operator[](i); }
const JsonToken& JsonToken::operator[](const string& i) const { return value_->operator[](i); }

size_t JsonToken::size() const noexcept { return value_->size(); }

bool operator==(const JsonToken& lhs, const JsonToken& rhs) noexcept {
	if (lhs.type() != rhs.type()) return false;

	switch (lhs.type()) {
	case JsonType::kNull: return true;
	case JsonType::kBool: return lhs.toBool() == rhs.toBool();
	case JsonType::kNumber: return lhs.toNumber() == rhs.toNumber();
	case JsonType::kString: return lhs.toString() == rhs.toString();
	case JsonType::kArray: return lhs.toArray() == rhs.toArray();
	case JsonType::kObject: return lhs.toObject() == rhs.toObject();
	}
	assert(0);
}
inline bool operator!=(const JsonToken& lhs, const JsonToken& rhs) noexcept {
	return !(lhs == rhs);
}