/**
Copyright (c) <2014> <Julian Kenwood>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**/

#include <string>
#include <unordered_map>
#include <vector>
#include <exception>
#include <stdexcept>
#include <cctype>
#include <unordered_set>
#include <limits>
#include <regex>
#include "variadic_string.h"
#include "format_string.h"
#include <cstring>
#include <typeinfo>


/** The actual voodoo is here. **/ 

namespace TypeSafeJSON
{
    /** Utility functions **/
    static inline bool is_valid_unicode(int codepoint)
    {
        return !(codepoint < 0x00000000 || codepoint > 0x0010FFFF);
    }

    static inline std::string escape_string(const std::string& input)
    {
        std::string result = "";
        for(int i = 0; i < int(input.size()); ++i)
        {
            switch(input[i]) 
            {
                case '\n':
                    result.append("\n");
                    break;
                case '\\':
                    result.append("\\\\");
                    break;
                case '\"':
                    result.append("\\\"");
                    break;
                default:
                    result.append(1, input[i]);
                    break;
            }
        }
        return result;
    }
    
    static inline int hex_to_dec(char hex)
    {
        if(hex >= '0' && hex <= '9')
            return hex - '0';
        else if(hex >= 'a' && hex <= 'f')
            return hex - 'a' + 10;
        else if(hex >= 'A' && hex <= 'F')
            return hex - 'A' + 10;
        else
            return -1;
    }
    
    template <typename Input>
    static inline void eat_whitespace(Input& input)
    {
        while(true)
        {
            int expected_ws = input.getc();
            
            if(!isspace(expected_ws))
            {
                input.ungetc(expected_ws);
                break;
            }
        }
    }
    
    static inline std::string codepoint_to_utf8(int codepoint)
    {
        if(codepoint < 0)
            return "";
        
        if(codepoint <= 127)
        {
            return std::string(1, codepoint);
        }
        else if(codepoint <= 0x000007FF)
        {
            std::string result;
            const unsigned char char1 = (0xC0 | ((codepoint & 0x7C0) >> 6));
            const unsigned char char2 = (0x80 | ((codepoint & 0x03F)));
            
            result.push_back(char1);
            result.push_back(char2);
            
            return result;
        }
        else if(codepoint <= 0x000007FF)
        {
            std::string result;
            
            const unsigned char char1 = (0xC0 | ((codepoint & 0x7C0) >> 6));
            const unsigned char char2 = (0x80 | ((codepoint & 0x03F)));
            
            result.push_back(char1);
            result.push_back(char2);
            
            return result;
        }
        else if(codepoint <= 0x0000FFFF)
        {
            std::string result;
            
            const unsigned char char1 = (0xE0 | ((codepoint & 0xF000) >> 12));
            const unsigned char char2 = (0x80 | ((codepoint & 0x0FC0) >> 6));
            const unsigned char char3 = (0x80 | ((codepoint & 0x003F)));
            
            result.push_back(char1);
            result.push_back(char2);
            result.push_back(char3);
            
            return result;
        }
        else if(codepoint <= 0x0010FFFF)
        {
            std::string result;
            
            const unsigned char char1 = (0xF0 | ((codepoint & 0x1C0000) >> 18));
            const unsigned char char2 = (0x80 | ((codepoint & 0x03F000) >> 12));
            const unsigned char char3 = (0x80 | ((codepoint & 0x000FC0) >> 6));
            const unsigned char char4 = (0x80 | ((codepoint & 0x00003F)));
            
            result.push_back(char1);
            result.push_back(char2);
            result.push_back(char3);
            result.push_back(char4);
            
            return result;
        }
        else
        {
            return "";
        }
    }
    
    /** Some exceptions. **/
    
    class BadJSONFormatException : public std::exception
    {
    private:
        std::string message;
        int position;
        
    public:
        BadJSONFormatException(std::string message = "Unspecified bad JSON format exception occurred", int position = -1) : message(message), position(position)
        {
        }
        
        virtual int get_position() const noexcept
        {
            return position;
        }
        virtual const char* what() const noexcept
        {
            return message.c_str();
        }
        
        virtual ~BadJSONFormatException() throw ()
        {
        }
    };
    
    class JSONValidationException : public std::exception
    {
    private:
        std::string message;
        int position;
        
    public:
        JSONValidationException(std::string message = "JSON validation exception", int position = -1) : message(message), position(position)
        {
        }
        
        virtual int get_position() const noexcept
        {
            return position;
        }
        virtual const char* what() const noexcept
        {
            return message.c_str();
        }
        
        virtual ~JSONValidationException() throw ()
        {
        }
    };
    
    /** A way to abstract out input sources. **/
    template <typename T>
    class InputSource
    {
    };
    
    template <>
    class InputSource<FILE*>
    {
    private:
        FILE*& fptr;
        
    public:
        InputSource(FILE*& in) : fptr(in)
        {
        }
        
        int getc()
        {
            return fgetc(fptr);
        }
        
        void ungetc(int chr)
        {
            ::ungetc(chr, fptr);
        }
        
        template <typename T>
        T get_value()
        {
            T x;
            int scan_result = fscanf(fptr, FormatTypes<T>::format.c_str(), &x);
            if(scan_result != 1)
                throw BadJSONFormatException("Failed to parse number", tell());
            return x;
        }
        
        int tell()
        {
            return ftell(fptr);
        }
    };
    
    template <>
    class InputSource<std::string>
    {
    private:
        std::string& input;
        int pos;
        
    public:
        InputSource(std::string& in) : input(in), pos(0)
        {
        }
        
        int getc()
        {
            return (pos == int(input.size())) ? EOF : input[pos++];
        }
        
        void ungetc(int chr)
        {
            --pos;
        }
        
        template <typename T>
        T get_value()
        {
            T x;
            int length = 0;
            int scan_result = sscanf(input.c_str() + pos, FormatTypes<T, void>::format.c_str(), &x, &length);
            pos += length;
            if(scan_result != 1)
                throw BadJSONFormatException("Failed to parse number", tell());
            return x;
        }
        
        int tell()
        {
            return pos;
        }
    };
    
    /** Number Implementation **/
    
    /// Number Validators
    template <typename Type>
    class DefaultNumberValidator
    {
    public:
        static bool check(const Type& value)
        {
            return true;
        }
    };
    
    template <typename Type>
    class PositiveNumberValidator
    {
        static bool check(const Type& value)
        {
            return value > 0;
        }
    };
    
    template <typename Type>
    class NonNegativeNumberValidator
    {
        static bool check(const Type& value)
        {
            return value >= 0;
        }
    };
    
    template <typename Type>
    class ZeroNumberValidator
    {
        static bool check(const Type& value)
        {
            return value == 0;
        }
    };
    
    template <typename Type>
    class NonPositiveNumberValidator
    {
        static bool check(const Type& value)
        {
            return value <= 0;
        }
    };
    
    template <typename Type>
    class NegativeNumberValidator
    {
        static bool check(const Type& value)
        {
            return value < 0;
        }
    };
    
    /// Number Factory to parse JSON Numbers represented as Type. Type will default to long double. This should always work even if Type is integral.
    template <typename Type = long double, template <typename> class Validator = DefaultNumberValidator>
    class JSONNumberFactory
    {
    public:
        typedef Type ValueType;

        template <typename Input>
        static ValueType parse_impl(Input& input)
        {
            const ValueType value = input.template get_value<ValueType>();
            if(!Validator<Type>::check(value))
                throw JSONValidationException();
            
            return value;
        }
        
        template <typename InputType>
        static ValueType parse(InputType& input)
        {
            InputSource<InputType> source(input);
            return JSONNumberFactory<Type, Validator>::parse_impl(source);
        }
        
        static bool is_valid_object(const ValueType& value)
        {
            return Validator<ValueType>::check(value);
        }
        
        static void validate(const ValueType& value)
        {
            if(!Validator<ValueType>::check(value))
                throw JSONValidationException();
        }
        
        static std::vector<std::string> as_json_lines(const ValueType& value)
        {
            return std::vector<std::string>{std::to_string(value)};
        }
        
        static std::string as_json(const ValueType& value)
        {
            std::string result = "";
            const std::vector<std::string>& lines = JSONNumberFactory<Type, Validator>::as_json_lines(value);
            
            for(int i = 0; i < int(lines.size()); ++i)
                if(i != lines.size() - 1)
                    result += lines[i] + "\n";
                else
                    result += lines[i];
            
            return result;
        }
    };
    
    /** String Implementation **/
    
    /// String validators
    template <typename Type>
    class DefaultStringValidator
    {
    public:
        static bool check(const Type& value)
        {
            return true;
        }
    };
    
    template <typename Type>
    class NonEmptyStringValidator
    {
    public:
        static bool check(const Type& value)
        {
            return !value.empty();
        }
    };
    
    /// String Factory to parse JSON Strings represented as Type. Type will default to std::string
    template <typename Type = std::string, template <typename> class Validator = DefaultStringValidator>
    class JSONStringFactory
    {
    public:
        typedef Type ValueType;
        
        template <typename Input>
        static ValueType parse_impl(Input& input)
        {
            ValueType value = "";
            
            int expected_double_quote = input.getc();
            if(expected_double_quote != '"')
                throw BadJSONFormatException("Expected opening double quote", input.tell());
            
            while(true)
            {
                int chr = input.getc();
                
                if(iscntrl(chr))
                    throw BadJSONFormatException("Control character found in string", input.tell());
                
                
                if(chr == '\\')
                {
                    int next_chr = input.getc();
                    
                    if(next_chr == '"')
                    {
                        value.push_back('"');
                    }
                    else if(next_chr == '\\')
                    {
                        value.push_back('\\');
                    }
                    else if(next_chr == '/')
                    {
                        value.push_back('/');
                    }
                    else if(next_chr == 'b')
                    {
                        value.push_back('\b');
                    }
                    else if(next_chr == 'f')
                    {
                        value.push_back('\f');
                    }
                    else if(next_chr == 'n')
                    {
                        value.push_back('\n');
                    }
                    else if(next_chr == 'r')
                    {
                        value.push_back('\r');
                    }
                    else if(next_chr == 't')
                    {
                        value.push_back('\t');
                    }
                    else if(next_chr == 'u')
                    {
                        int hex1 = input.getc();
                        if(!isxdigit(hex1))
                            throw BadJSONFormatException("Invalid hexadecimal digit in unicode character code", input.tell());
                        int hex2 = input.getc();
                        if(!isxdigit(hex2))
                            throw BadJSONFormatException("Invalid hexadecimal digit in unicode character code", input.tell());
                        int hex3 = input.getc();
                        if(!isxdigit(hex3))
                            throw BadJSONFormatException("Invalid hexadecimal digit in unicode character code", input.tell());
                        int hex4 = input.getc();
                        if(!isxdigit(hex4))
                            throw BadJSONFormatException("Invalid hexadecimal digit in unicode character code", input.tell());
                        
                        unsigned int dec1 = hex_to_dec(hex1);
                        unsigned int dec2 = hex_to_dec(hex2);
                        unsigned int dec3 = hex_to_dec(hex3);
                        unsigned int dec4 = hex_to_dec(hex4);
                        
                        unsigned int dec = dec4 + 16*(dec3 + 16*(dec2 + 16*dec1));
                        
                        if(!is_valid_unicode(dec))
                            throw BadJSONFormatException("Invalid unicode codepoint in escape code", input.tell());
                            
                        const std::string& chrs = codepoint_to_utf8(dec);
                        value += chrs;
                    }
                    else
                    {
                        value.push_back('\\');
                        input.ungetc('\\');
                    }
                }
                else if(chr == '"')
                {
                    break;
                }
                else
                {
                    value.push_back(chr);
                }
            }
            
            if(!Validator<Type>::check(value))
                throw JSONValidationException();
            return value;
        }
        
        template <typename InputType>
        static ValueType parse(InputType& input)
        {
            InputSource<InputType> source(input);
            return JSONStringFactory<Type, Validator>::parse_impl(source);
        }
        
        static bool is_valid_object(const ValueType& value)
        {
            return Validator<ValueType>::check(value);
        }
        
        static void validate(const ValueType& value)
        {
            if(!Validator<ValueType>::check(value))
                throw JSONValidationException();
        }
        
        static std::vector<std::string> as_json_lines(const ValueType& value)
        {
            return std::vector<std::string>{escape_string(value)};
        }
        
        static std::string as_json(const ValueType& value)
        {
            std::string result = "";
            const std::vector<std::string>& lines = JSONStringFactory<Type, Validator>::as_json_lines(value);
            
            for(int i = 0; i < int(lines.size()); ++i)
                result += lines[i] + "\n";
            
            return result;
        }
    };
    
    /** Boolean Implementation **/
    
    /// Boolean Validators
    template <typename Type>
    class DefaultBooleanValidator
    {
    public:
        static bool check(const Type& value)
        {
            return true;
        }
    };
    
    /// A factory of JSONBooleans represented as Type. Type defaults to bool
    template <typename Type = bool, template <typename> class Validator = DefaultBooleanValidator>
    class JSONBooleanFactory
    {
    public:
        typedef Type ValueType;
        
        template <typename Input>
        static ValueType parse_impl(Input& input)
        {
            ValueType value;
            
            int expected_t_or_f = input.getc();
            
            if(expected_t_or_f == 't')
            {
                int expected_r = input.getc();
                if(expected_r != 'r')
                    throw BadJSONFormatException("Unrecognised boolean", input.tell());
                
                int expected_u = input.getc();
                if(expected_u != 'u')
                    throw BadJSONFormatException("Unrecognised boolean", input.tell());
                
                int expected_e = input.getc();
                if(expected_e != 'e')
                    throw BadJSONFormatException("Unrecognised boolean", input.tell());
                
                value = true;
            }
            else if(expected_t_or_f == 'f')
            {
                int expected_a = input.getc();
                if(expected_a != 'a')
                    throw BadJSONFormatException("Unrecognised boolean", input.tell());
                
                int expected_l = input.getc();
                if(expected_l != 'l')
                    throw BadJSONFormatException("Unrecognised boolean", input.tell());
                
                int expected_s = input.getc();
                if(expected_s != 's')
                    throw BadJSONFormatException("Unrecognised boolean", input.tell());
                
                int expected_e = input.getc();
                if(expected_e != 'e')
                    throw BadJSONFormatException("Unrecognised boolean", input.tell());
                
                value = false;
            }
            else
            {
                throw BadJSONFormatException("Unrecognised boolean", input.tell());
            }
            
            if(!Validator<Type>::check(value))
                throw JSONValidationException();
            
            return value;
        }
        
        template <typename InputType>
        static ValueType parse(InputType& input)
        {
            InputSource<InputType> source(input);
            return JSONBooleanFactory<Type, Validator>::parse_impl(source);
        }
        
        static bool is_valid_object(const ValueType& value)
        {
            return Validator<ValueType>::check(value);
        }
        
        static void validate(const ValueType& value)
        {
            if(!Validator<ValueType>::check(value))
                throw JSONValidationException();
        }
        
        static std::vector<std::string> as_json_lines(const ValueType& value)
        {
            return std::vector<std::string>{value ? "true" : "false"};
        }
        
        static std::string as_json(const ValueType& value)
        {
            std::string result = "";
            const std::vector<std::string>& lines = JSONBooleanFactory<Type, Validator>::as_json_lines(value);
            
            for(int i = 0; i < int(lines.size()); ++i)
                result += lines[i] + "\n";
            
            return result;
        }
    };
    
    /** Array Implementation **/
    
    /// Array Validators
    template <typename Type>
    class DefaultArrayValidator
    {
    public:
        static bool check(const Type& value)
        {
            return true;
        }
    };
    
    template <typename Type>
    class UniqueArrayValidator
    {
    public:
        static bool check(const Type& value)
        {
            std::unordered_set<typename Type::value_type> seen_items;
            for(auto it = value.begin(); it != value.end(); ++it)
            {
                bool insert_happened = seen_items.insert(*it).second;
                if(!insert_happened)
                    return false;
            }
            return true;
        }
    };
    
    /// Creates a homogenous JSONArray of values of type T represented by Container which will default to std::vector of Ts.
    template <typename T, template <typename> class Validator = DefaultArrayValidator, typename Container = std::vector<typename T::ValueType>>
    class JSONArrayFactory
    {
    public:
        typedef Container ValueType;
        
        template <typename Input>
        static ValueType parse_impl(Input& input)
        {
            ValueType value;
            
            eat_whitespace(input);
            int expected_bracket = input.getc();
            
            if(expected_bracket != '[')
                throw BadJSONFormatException("Expected opening brace", input.tell());
            eat_whitespace(input);
            
            while(true)
            {
                typename T::ValueType entry = T::parse_impl(input);
                value.push_back(entry);
                
                int is_comma_or_close_bracket = input.getc();
                
                if(is_comma_or_close_bracket == ']')
                    break;
                else if(is_comma_or_close_bracket != ',')
                    throw BadJSONFormatException("Expected comma or closing bracket", input.tell());
                
                eat_whitespace(input);
            }
            
            if(!Validator<Container>::check(value))
                throw JSONValidationException();
            
            return value;
        }
        
        template <typename InputType>
        static ValueType parse(InputType& input)
        {
            InputSource<InputType> source(input);
            return JSONArrayFactory<T, Validator, Container>::parse_impl(source);
        }
        
        static bool is_valid_object(const ValueType& value)
        {
            return Validator<ValueType>::check(value);
        }
        
        static void validate(const ValueType& value)
        {
            if(!Validator<ValueType>::check(value))
                throw JSONValidationException();
        }
        
        static std::vector<std::string> as_json_lines(const ValueType& value)
        {
            if(value.empty()) 
            {
                return std::vector<std::string>{"[]"};
            }
            else
            {
                std::vector<std::string> result{"["};
                
                for(int i = 0; i < int(value.size()); ++i)
                {
                    const std::vector<std::string>& rep = T::as_json_lines(value[i]);
                    for(int j = 0; j < int(rep.size()); ++j)
                        result.push_back("  " + rep[j]);
                    
                    if(i != int(value.size()) - 1)
                        result.back() += ",";
                }
                result.push_back("]");
                return result;
            }
        }
        
        static std::string as_json(const ValueType& value)
        {
            std::string result = "";
            const std::vector<std::string>& lines = JSONArrayFactory<T, Validator, Container>::as_json_lines(value);
            
            for(int i = 0; i < int(lines.size()); ++i)
                result += lines[i] + "\n";
            
            return result;
        }
    };
    
    /** Object Implementation - this part's the killer. **/
    
    /** Implementation required for binary search tree of fields. **/
    template <int switcher, typename Negative, typename Zero, typename Positive>
    class Conditional3Way
    {
    };
    
    template <typename Negative, typename Zero, typename Positive>
    class Conditional3Way<-1, Negative, Zero, Positive>
    {
    public:
        typedef Negative type;
    };
    
    template <typename Negative, typename Zero, typename Positive>
    class Conditional3Way<0, Negative, Zero, Positive>
    {
    public:
        typedef Zero type;
    };
    
    template <typename Negative, typename Zero, typename Positive>
    class Conditional3Way<1, Negative, Zero, Positive>
    {
    public:
        typedef Positive type;
    };
    
    template<bool truth, typename T, typename U> 
    class copy_if_true
    {
    };
    
    template<typename T, typename U> 
    class copy_if_true<true, T, U>
    {
    public:
        static void copy(T& output, const U& input)
        {
            output = input;
        }
    };
    
    template<typename T, typename U> 
    class copy_if_true<false, T, U>
    {
    public:
        static void copy(T& output, const U& input)
        {
        }
    };
    
    template <typename... Args>
    class JSONSetFieldCollection
    {
    };
    
    template <>
    class JSONSetFieldCollection<>
    {
    public:
        static constexpr int sz = 0;
        
        static std::string to_string()
        {
            return "";
        }
    };
    
    template <typename T, char... vs, typename... Args>
    class JSONSetFieldCollection<NamedType<T, vs...>, Args...>
    {
    public:
        static constexpr int sz = 1 + sizeof...(Args);
        
        static std::string to_string()
        {
            return "<" + std::string(CompileStr<vs...>::value) + ": " + typeid(T).name() + ">\n" + JSONSetFieldCollection<Args...>::to_string();
        }
    };
    
    template <typename FieldA, typename FieldB> 
    class Concat
    {
    };

    template <typename... ATypes, typename... BTypes>
    class Concat<JSONSetFieldCollection<ATypes...>, JSONSetFieldCollection<BTypes...>>
    {
    public:
        typedef JSONSetFieldCollection<ATypes..., BTypes...> type;
    };
    
    template <typename FieldA, typename FieldB>
    class FieldComparatorHelper
    {
    };
    
    template <typename A, typename B>
    class FieldComparatorHelper<NamedType<A>, NamedType<B>>
    {
    public:
        static constexpr int result = 0;
    };
    
    template <typename A, typename B, char vbfirst, char... vbs>
    class FieldComparatorHelper<NamedType<A>, NamedType<B, vbfirst, vbs...>>
    {
    public:
        static constexpr int result = -1;
    };
    
    template <typename A, char vafirst, char... vas, typename B>
    class FieldComparatorHelper<NamedType<A, vafirst, vas...>, NamedType<B>>
    {
    public:
        static constexpr int result = 1;
    };
    
    template <typename A, char matching, char... vas, typename B, char... vbs>
    class FieldComparatorHelper<NamedType<A, matching, vas...>, NamedType<B, matching, vbs...>>
    {
    public:
        static constexpr int result = FieldComparatorHelper<NamedType<A, vas...>, NamedType<B, vbs...>>::result;
    };
    
    template <typename A, char vafirst, char... vas, typename B, char vbfirst, char... vbs>
    class FieldComparatorHelper<NamedType<A, vafirst, vas...>, NamedType<B, vbfirst, vbs...>>
    {
    public:
        static constexpr int result = vafirst < vbfirst ? -1 : 1;
    };
    
    template <typename Field, char... vs>
    class FieldNameReverser
    {
    };
    
    template <typename T, char in_first, char... in_vs, char... out_vs>
    class FieldNameReverser<NamedType<T, in_first, in_vs...>, out_vs...>
    {
    public:
        typedef typename FieldNameReverser<NamedType<T, in_vs...>, out_vs..., in_first>::result result;
    };
    
    template <typename T, char... out_vs>
    class FieldNameReverser<NamedType<T>, out_vs...>
    {
    public:
        typedef NamedType<T, out_vs...> result;
    };
    
    template <typename FieldA, typename FieldB>
    class FieldComparator
    {
    public:
        static constexpr int result = FieldComparatorHelper<typename FieldNameReverser<FieldA>::result, 
                                                            typename FieldNameReverser<FieldB>::result>::result;
    };
    
    template <int cur, typename Left, typename Remainder>
    class MiddleSplitterHelper
    {
    };
    
    template <typename... LeftFields, typename First, typename... Remainder>
    class MiddleSplitterHelper<0, JSONSetFieldCollection<LeftFields...>, JSONSetFieldCollection<First, Remainder...>>
    {
    public:
        typedef JSONSetFieldCollection<LeftFields...> left;
        typedef First middle;
        typedef JSONSetFieldCollection<Remainder...> right;
    };
    
    template <int cur, typename... LeftFields, typename First, typename... Remainder>
    class MiddleSplitterHelper<cur, JSONSetFieldCollection<LeftFields...>, JSONSetFieldCollection<First, Remainder...>>
    {
    public:
        typedef MiddleSplitterHelper<cur - 1, JSONSetFieldCollection<LeftFields..., First>, JSONSetFieldCollection<Remainder...>> helper;
        typedef typename helper::left left;
        typedef typename helper::middle middle;
        typedef typename helper::right right;
    };
    
    template <typename Fields>
    class MiddleSplitter
    {
    };
    
    template <typename First, typename... Rest>
    class MiddleSplitter<JSONSetFieldCollection<First, Rest...>>
    {
    public:
        typedef MiddleSplitterHelper<(sizeof...(Rest) + 1)/2, JSONSetFieldCollection<>, JSONSetFieldCollection<First, Rest...>> helper;
        typedef typename helper::left left;
        typedef typename helper::middle middle;
        typedef typename helper::right right;
    };
    
    template <typename Pivot, typename Fields>
    class Partition
    {
    };
    
    template <typename PivotT, char... pivotvs, typename FirstT, char... firstvs, typename... Fields>
    class Partition<NamedType<PivotT, pivotvs...>, JSONSetFieldCollection<NamedType<FirstT, firstvs...>, Fields...>>
    {
    public:
        typedef Partition<NamedType<PivotT, pivotvs...>, JSONSetFieldCollection<Fields...>> next;
        static constexpr int cmp = FieldComparator<NamedType<PivotT, pivotvs...>, NamedType<FirstT, firstvs...>>::result;
        
        typedef typename std::conditional<cmp == 1, JSONSetFieldCollection<NamedType<FirstT, firstvs...>>, JSONSetFieldCollection<>>::type cur_lt;
        typedef typename std::conditional<cmp == 0, JSONSetFieldCollection<NamedType<FirstT, firstvs...>>, JSONSetFieldCollection<>>::type cur_eq;
        typedef typename std::conditional<cmp == -1, JSONSetFieldCollection<NamedType<FirstT, firstvs...>>, JSONSetFieldCollection<>>::type cur_gt;
        
        typedef typename Concat<typename next::lt, cur_lt>::type lt;
        typedef typename Concat<typename next::eq, cur_eq>::type eq;
        typedef typename Concat<typename next::gt, cur_gt>::type gt;
    };
    
    template <typename PivotT, char... pivotvs>
    class Partition<NamedType<PivotT, pivotvs...>, JSONSetFieldCollection<>>
    {
    public:
        typedef JSONSetFieldCollection<> lt;
        typedef JSONSetFieldCollection<> eq;
        typedef JSONSetFieldCollection<> gt;
    };
    
    template <typename Fields>
    class FieldSorter
    {
    public:
        typedef typename MiddleSplitter<Fields>::middle Pivot;
        typedef Partition<Pivot, Fields> partition;
        typedef typename Concat<typename FieldSorter<typename partition::lt>::type, typename partition::eq>::type _sub;
        typedef typename Concat<_sub, typename FieldSorter<typename partition::gt>::type>::type type;
    };
    
    template <>
    class FieldSorter<JSONSetFieldCollection<>>
    {
    public:
        typedef JSONSetFieldCollection<> type;
    };
    
    template <typename Fields>
    class FieldTreeHelper
    {
    public:
        typedef typename MiddleSplitter<Fields>::left LeftTree;
        typedef typename MiddleSplitter<Fields>::middle Root;
        typedef typename MiddleSplitter<Fields>::right RightTree;
        
        FieldTreeHelper<LeftTree> left;
        typename Root::Type::ValueType root;
        FieldTreeHelper<RightTree> right;
        
        template <char... param_chrs>
        class ParameterType
        {
        public:
            static constexpr int cmp_val = FieldComparator<NamedType<int, param_chrs...>, Root>::result;
            typedef FieldTreeHelper<LeftTree> lt;
            typedef FieldTreeHelper<RightTree> gt;
            class Eq
            {
            public:
                class type
                {
                public:
                    typedef typename Root::Type::ValueType Type;
                };
            };
            
            typedef typename Conditional3Way<cmp_val, 
                                             typename lt::template ParameterType<param_chrs...>,
                                             Eq,
                                             typename gt::template ParameterType<param_chrs...>
                                            >::type::type type;
        };
        
        template <typename Input>
        void parse_against_parameter(std::string parameter_name, Input& input)
        {
            int cmp_val = strcmp(parameter_name.c_str(), Root::NameType::value);
            if(cmp_val == 0)
                root = Root::Type::parse_impl(input);
            else if(cmp_val < 0)
                left.parse_against_parameter(parameter_name, input);
            else if(cmp_val > 0)
                right.parse_against_parameter(parameter_name, input);
        }
        
        void path(std::string parameter_name)
        {
            printf("'%s' vs '%s'\n", parameter_name.c_str(), Root::NameType::value);
            
            int cmp_val = strcmp(parameter_name.c_str(), Root::NameType::value);
            if(cmp_val == 0)
                printf("Hit\n");
            else if(cmp_val < 0)
                left.path(parameter_name);
            else if(cmp_val > 0)
                right.path(parameter_name);
        }
        
        template <char... param_chrs>
        typename ParameterType<param_chrs...>::type::Type get() const
        {
            typename ParameterType<param_chrs...>::type::Type val;
            get_into<typename ParameterType<param_chrs...>::type::Type, param_chrs...>(val);
            return val;
        }
        
        template <typename T, char... param_chrs>
        void get_into(T& val) const
        {
            copy_if_true<std::is_same<CompileStr<param_chrs...>, typename Root::NameType>::value, T, typename Root::Type::ValueType>::copy(val, root);
            
            int cmp_val = strcmp(CompileStr<param_chrs...>::value, Root::NameType::value);
            if(cmp_val < 0)
                left.template get_into<T, param_chrs...>(val);
            else if(cmp_val > 0)
                right.template get_into<T, param_chrs...>(val);
        }
        
        template <typename T, char... param_chrs>
        void set(T& val)
        {
            copy_if_true<std::is_same<CompileStr<param_chrs...>, typename Root::NamedType>::value, typename Root::Type::ValueType, T>::copy(root, val);
            
            int cmp_val = strcmp(CompileStr<param_chrs...>::value, Root::NameType::value);
            if(cmp_val < 0)
                left.template set<T, param_chrs...>(val);
            else if(cmp_val > 0)
                right.template set<T, param_chrs...>(val);
        }
        
        bool is_empty() const
        {
            return false;
        }
        
        void as_json_fields(std::vector<std::string>& output_lines, bool is_first = true) const
        {
            left.as_json_fields(output_lines, is_first);
            if(!left.is_empty())
                is_first = false;
            
            if(!is_first)
                output_lines.back() += ",";
            
            std::vector<std::string> values = Root::Type::as_json_lines(root);
            std::string field_name = "  \"" + escape_string(Root::name) + "\": ";
            std::string first_line = field_name + values[0];
            output_lines.push_back(first_line);
            
            for(int i = 1; i < int(values.size()); ++i)
                output_lines.push_back("  " + values[i]);
            
            right.as_json_fields(output_lines, false);
        }
    };
    
    template <>
    class FieldTreeHelper<JSONSetFieldCollection<>>
    {
    public:
        class JSONNoSuchParameter
        {
        };
        
        template <char... param_chrs>
        class ParameterType
        {
        public:
            typedef JSONNoSuchParameter type;
        };
        
        template <typename Input>
        void parse_against_parameter(std::string parameter_name, Input& input)
        {
            throw BadJSONFormatException("Parameter was not found in structure: " + parameter_name, input.tell());
        }
        
        void path(std::string parameter_name)
        {
            printf("Miss\n");
        }
        
        template <char... param_chrs>
        typename ParameterType<param_chrs...>::type::Type get() const
        {
            throw std::logic_error("Parameter not found: " + CompileStr<param_chrs...>::value);
            typename ParameterType<param_chrs...>::type::Type val;
            get_into<typename ParameterType<param_chrs...>::type::Type, param_chrs...>(val);
            return val;
        }
        
        template <typename T, char... param_chrs>
        void get_into(T& val) const
        {
        }
        
        template <typename T, char... param_chrs>
        void set(T& val)
        {
            
        }
        
        bool is_empty() const
        {
            return true;
        }
        
        void as_json_fields(std::vector<std::string>& output_lines, bool is_first = true) const
        {
        }
    };
    
    template <typename Fields>
    class FieldTree : public FieldTreeHelper<typename FieldSorter<Fields>::type>
    {
    };
    /** End of Field BST Implementation **/
    
    /// JSONSet implementation, compile-time name to field mapping. Uses NamedType as its template arguments.
    template <typename... Args> 
    class JSONSet;
    
    template <> 
    class JSONSet<> : FieldTree<JSONSetFieldCollection<>>
    {
    public:
        /// find_missing_parameters is common
        static void find_missing_parameters(std::vector<std::string>& missing_fields, std::unordered_set<std::string> seen_parameters)
        {
        }
    };
    
    template <typename HeadType, char... chrs, typename... Rest> 
    class JSONSet<NamedType<HeadType, chrs...>, Rest...> : public FieldTree<JSONSetFieldCollection<NamedType<HeadType, chrs...>, Rest...>>
    {
    public:
        /// find_missing_parameters is common
        static void find_missing_parameters(std::vector<std::string>& missing_fields, std::unordered_set<std::string> seen_parameters)
        {
            if(seen_parameters.count(CompileStr<chrs...>::value) == 0)
                missing_fields.push_back(std::string(CompileStr<chrs...>::value));
            
            JSONSet<Rest...>::find_missing_parameters(missing_fields, seen_parameters);
        }
    };
    
    /// JSONSet Validators
    template <typename Type>
    class DefaultObjectValidator
    {
    public:
        static bool check(const Type& value)
        {
            return true;
        }
    };
    
    template <typename Type>
    class SillyObjectValidator
    {
    public:
        static bool check(const Type& value)
        {
            std::vector<long double> list = value.template get<str_to_list_4("list")>();
            std::string id = value.template get<str_to_list_2("id")>();
            int sum = 0;
            for(int i = 0; i < int(list.size()); ++i)
                sum += list[i];
            char chr_val = '0' + sum;
            return id.find(chr_val) != std::string::npos;
        }
    };
    
    /// A factory to parse these JSONSet types
    template <typename T, template <typename> class Validator = DefaultObjectValidator>
    class JSONObjectFactory
    {
    public:
        typedef T ValueType;
        
        template <typename Input>
        static ValueType parse_impl(Input& input)
        {
            T value;
            std::unordered_set<std::string> parameter_set;
            
            eat_whitespace(input);
            char expected_brace = input.getc();
            if(expected_brace != '{')
                throw BadJSONFormatException("Expected an opening brace", input.tell());
            
            bool is_first_run = true;
            while(true)
            {
                eat_whitespace(input);
                if(!is_first_run)
                {
                    char expected_comma_or_brace = input.getc();
                    if(expected_comma_or_brace != ',' && expected_comma_or_brace != '}')
                        throw BadJSONFormatException("Expected a colon or comma", input.tell());
                    else if(expected_comma_or_brace == '}')
                        break;
                    
                }
                else
                {
                    int potential_brace = input.getc();
                    if(potential_brace == '}')
                        break;
                    
                    input.ungetc(potential_brace);
                }
                
                eat_whitespace(input);
                
                std::string field_name = JSONStringFactory<std::string>::parse_impl(input);
                
                bool insert_happened = parameter_set.insert(field_name).second;
                if(!insert_happened)
                    throw BadJSONFormatException("The following field was duplicated: " + field_name, input.tell()); 
                
                eat_whitespace(input);
                char expected_colon = input.getc();
                eat_whitespace(input);
                
                if(expected_colon != ':')
                    throw BadJSONFormatException("A colon was expected.", input.tell());
                
                value.parse_against_parameter(field_name, input);
                
                is_first_run = false;
            }
            std::vector<std::string> missing_fields;
            T::find_missing_parameters(missing_fields, parameter_set);
            
            if(!missing_fields.empty())
            {
                std::string fields = "";
                for(int i = 0; i < int(missing_fields.size()) - 1; ++i)
                    fields += missing_fields[i] + ", ";
                fields += missing_fields.back();
                
                throw BadJSONFormatException("The following fields were not found in the input: " + fields, input.tell());
            }
            
            if(!Validator<T>::check(value))
                throw JSONValidationException();
            
            return value;
        }
        
        template <typename InputType>
        static ValueType parse(InputType& input)
        {
            InputSource<InputType> source(input);
            return JSONObjectFactory<T, Validator>::parse_impl(source);
        }
        
        static bool is_valid_object(const ValueType& value)
        {
            return Validator<ValueType>::check(value);
        }
        
        static void validate(const ValueType& value)
        {
            if(!Validator<ValueType>::check(value))
                throw JSONValidationException();
        }
        
        static std::vector<std::string> as_json_lines(const T& value)
        {
            std::vector<std::string> result = std::vector<std::string>(1, "{");
            value.as_json_fields(result);
            result.push_back("}");
            
            return result;
        }
        
        static std::string as_json(const T& value)
        {
            std::string result = "";
            const std::vector<std::string>& lines = JSONObjectFactory<T, Validator>::as_json_lines(value);
            
            for(int i = 0; i < int(lines.size()); ++i)
                result += lines[i] + "\n";
            
            return result;
        }
    };
    
    /// A factory that constructs the result to another type
    template <typename Type>
    class DefaultObjectDefinedClassValidator
    {
    public:
        static bool check(const Type& value)
        {
            return true;
        }
    };
    
    template <typename ObjectClass, template <typename> class Validator = DefaultObjectDefinedClassValidator>
    class JSONObjectToClassFactory
    {
    public:
        typedef ObjectClass ValueType;
        
        template <typename Input>
        static ValueType parse_impl(Input& input)
        {
            typename ObjectClass::Definition result = JSONObjectFactory<typename ObjectClass::Definition, Validator>::parse_impl(input);
            ValueType value = ValueType::Construct(result);
            
            if(!Validator<ValueType>::check(value))
                throw JSONValidationException();
            
            return value;
        }
        
        template <typename InputType>
        static ValueType parse(InputType& input)
        {
            InputSource<InputType> source(input);
            return JSONObjectToClassFactory<ObjectClass, Validator>::parse_impl(source);
        }
        
        static bool is_valid_object(const ValueType& value)
        {
            return Validator<ValueType>::check(value.as_object());
        }
        
        static void validate(const ValueType& value)
        {
            if(!Validator<ValueType>::check(value.as_object()))
                throw JSONValidationException();
        }
        
        static std::vector<std::string> as_json_lines(const ValueType& value)
        {
            return JSONObjectFactory<typename ObjectClass::Definition, Validator>::as_json_lines(value.as_object());
        }
        
        static std::string as_json(const ValueType& value)
        {
            return JSONObjectFactory<typename ObjectClass::Definition, Validator>::as_json(value.as_object());
        }
    };
}