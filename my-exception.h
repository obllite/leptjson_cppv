#pragma once
#include<stdexcept>
using namespace std;
class myException final : public runtime_error {
public:
	explicit myException(const string& errMsg) : runtime_error(errMsg) {};
	const char* what() const noexcept override { return runtime_error::what(); };
};