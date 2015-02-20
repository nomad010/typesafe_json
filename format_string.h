/**
Copyright (c) <2014> <Julian Kenwood>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**/

#pragma once
#include <string>

/** Utility templates for format strings. **/

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