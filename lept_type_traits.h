#ifndef LEPT_TPYE_TRAITS_H__
#define LEPT_TPYE_TRAITS_H__
#include "lept_value.h"
using namespace::std;
// There are the implementation of type traits which can extract the type of lept_value

// Extract null type
template<typename T>
struct is_null
{	static const bool value = false;};

template<>
struct is_null<lept_value_null>
{	static const bool value = true;};

// Extract false type
template<typename T>
struct is_false
{	static const bool value = false;};

template<>
struct is_false<lept_value_false>
{	static const bool value = true;};

 // Extract true type 
template<typename T>
struct is_true
{	static const bool value = false;};

template<>
struct is_true<lept_value_true>
{	static const bool value = true;};

// Extract number type
template<typename T>
struct is_number
{	static const bool value = false;};

template<>
struct is_number<lept_value_num>
{	static const bool value = true;};

// Extract string type
template<typename T>
struct is_string
{	static const bool value = false;};

template<>
struct is_string<lept_value_str>
{	static const bool value = true;};

#endif