#pragma once
#include <string>
#include <cstring>
template <typename T>
class Individual
{
};

template <>
class Individual<int>
{
public:
    static const std::string format;
};
const std::string Individual<int>::format = "%d";

template <>
class Individual<long double>
{
public:
    static const std::string format;
};
const std::string Individual<long double>::format = "%Lf";

template <>
class Individual<void>
{
public:
    static const std::string format;
};
const std::string Individual<void>::format = "%n";

template <typename... T>
class FormatTypes;

template <>
class FormatTypes<>
{
public:
    static const std::string format;
};
const std::string FormatTypes<>::format = "";

template <typename First, typename... Rest>
class FormatTypes<First, Rest...>
{
public:
    static const std::string format;
};
template <typename First, typename... Rest>
const std::string FormatTypes<First, Rest...>::format = Individual<First>::format + FormatTypes<Rest...>::format;