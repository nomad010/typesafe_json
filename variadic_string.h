/**
Copyright (c) <2014> <Julian Kenwood>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**/

#pragma once

#define str_to_list_1(s) s[0]
#define str_to_list_2(s) str_to_list_1(s), s[1]
#define str_to_list_3(s) str_to_list_2(s), s[2]
#define str_to_list_4(s) str_to_list_3(s), s[3]
#define str_to_list_5(s) str_to_list_4(s), s[4]
#define str_to_list_6(s) str_to_list_5(s), s[5]
#define str_to_list_7(s) str_to_list_6(s), s[6]
#define str_to_list_8(s) str_to_list_7(s), s[7]
#define str_to_list_9(s) str_to_list_8(s), s[8]
#define str_to_list_10(s) str_to_list_9(s), s[9]
#define str_to_list_11(s) str_to_list_10(s), s[10]
#define str_to_list_12(s) str_to_list_11(s), s[11]
#define str_to_list_13(s) str_to_list_12(s), s[12]
#define str_to_list_14(s) str_to_list_13(s), s[13]
#define str_to_list_15(s) str_to_list_14(s), s[14]
#define str_to_list_16(s) str_to_list_15(s), s[15]
#define str_to_list_17(s) str_to_list_16(s), s[16]
#define str_to_list_18(s) str_to_list_17(s), s[17]
#define str_to_list_19(s) str_to_list_18(s), s[18]
#define str_to_list_20(s) str_to_list_19(s), s[19]
#define str_to_list_21(s) str_to_list_20(s), s[20]
#define str_to_list_22(s) str_to_list_21(s), s[21]
#define str_to_list_23(s) str_to_list_22(s), s[22]
#define str_to_list_24(s) str_to_list_23(s), s[23]
#define str_to_list_25(s) str_to_list_24(s), s[24]
#define str_to_list_26(s) str_to_list_25(s), s[25]
#define str_to_list_27(s) str_to_list_26(s), s[26]
#define str_to_list_28(s) str_to_list_27(s), s[27]
#define str_to_list_29(s) str_to_list_28(s), s[28]
#define str_to_list_30(s) str_to_list_29(s), s[29]
#define str_to_list_31(s) str_to_list_30(s), s[30]
#define str_to_list_32(s) str_to_list_31(s), s[31]

template <char... Chrs>
class CompileStr {
public:
  static const char value[sizeof...(Chrs) + 1];
};
template<char... Chrs> const char CompileStr<Chrs...>::value[sizeof...(Chrs)+1] = {Chrs..., '\0'};

template <typename T, char... Chrs>
class NamedType
{
public:
    typedef T Type;
    typedef CompileStr<Chrs...> NameType;
    
    static const char name[sizeof...(Chrs) + 1];
};

template<typename T, char... Chrs> const char NamedType<T, Chrs...>::name[sizeof...(Chrs)+1] = {Chrs..., '\0'};