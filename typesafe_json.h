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

namespace TypeSafeJSON
{
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
        
        long double get_number()
        {
            long double x = 0;
            int scan_result = fscanf(fptr, "%Lf", &x);
            if(scan_result != 1)
                throw BadJSONFormatException("Failed to parse number", tell());
            return x;
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
        
        long double get_number()
        {
            long double x = 0;
            int length = 0;
            int scan_result = sscanf(input.c_str() + pos, "%Lf%n", &x, &length);
            pos += length;
            if(scan_result != 1)
                throw BadJSONFormatException("Failed to parse number", tell());
            return x;
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
    
    enum JSONNumberFactoryCapabilities
    {
        UnsetNumberCapability = 0,
        MinimumNumberCapability = 1,
        ExclusiveMinimumNumberCapability = 2,
        MaximumNumberCapability = 4,
        ExclusiveMaximumNumberCapability = 8
    };
    
    template <bool check_lower = false, bool exclusive_bound = false, long long int lower_bound = 0>
    class CheckLower;
    
    template <bool exclusive_bound, long long int lower_bound>
    class CheckLower<true, exclusive_bound, lower_bound>
    {
    public:
        template <typename T>
        static bool valid(const T& input)
        {
            return input > lower_bound || (input == lower_bound && !exclusive_bound);
        }
    };
    
    template <bool exclusive_bound, long long int lower_bound>
    class CheckLower<false, exclusive_bound, lower_bound>
    {
    public:
        template <typename T>
        static bool valid(const T& input)
        {
            return true;
        }
    };
    
    template <bool check_upper = false, bool exclusive_bound = false, long long int lower_bound = 0>
    class CheckUpper;
    
    template <bool exclusive_bound, long long int lower_bound>
    class CheckUpper<true, exclusive_bound, lower_bound>
    {
    public:
        template <typename T>
        static bool valid(const T& input)
        {
            return input < lower_bound || (input == lower_bound && !exclusive_bound);
        }
    };
    
    template <bool exclusive_bound, long long int lower_bound>
    class CheckUpper<false, exclusive_bound, lower_bound>
    {
    public:
        template <typename T>
        static bool valid(const T& input)
        {
            return true;
        }
    };
    
    template <JSONNumberFactoryCapabilities requirements,
              long long int arg1 = -1,
              long long int arg2 = -1>
    class CheckNumberRequirements
    {
    public:
        template <typename T>
        static bool valid(const T& input)
        {
            constexpr bool perform_minimum = requirements & MinimumNumberCapability;
            constexpr bool minimum_exclusive = requirements & ExclusiveMinimumNumberCapability;
            constexpr bool perform_maximum = requirements & MaximumNumberCapability;
            constexpr bool maximum_exclusive = requirements & ExclusiveMaximumNumberCapability;
            
            if(perform_minimum)
            {
                if(perform_maximum)
                    return CheckLower<true, minimum_exclusive, arg1>::valid(input) && CheckUpper<true, maximum_exclusive, arg2>::valid(input);
                else
                    return CheckLower<true, minimum_exclusive, arg1>::valid(input);
            }
            else
            {
                if(perform_maximum)
                    return CheckUpper<true, maximum_exclusive, arg1>::valid(input);
                else
                    return true;
            }
        }
    };
    
    template <JSONNumberFactoryCapabilities requirements,
              long long int arg1 = 0,
              long long int arg2 = 0>
    class JSONNumberFactory
    {
    public:
        typedef long double ValueType;
        
        template <typename Input>
        static ValueType parse_impl(Input& input)
        {
            const ValueType value = input.template get_value<ValueType>();
            if(!CheckNumberRequirements<requirements, arg1, arg2>::valid(value))
                throw JSONValidationException();
            
            return value;
        }
        
        template <typename InputType>
        static ValueType parse(InputType& input)
        {
            InputSource<InputType> source(input);
            return JSONNumberFactory<requirements, arg1, arg2>::parse_impl(source);
        }
        
        static std::vector<std::string> as_json_lines(const ValueType& value)
        {
            return std::vector<std::string>{std::to_string(value)};
        }
        
        static std::string as_json(const ValueType& value)
        {
            std::string result = "";
            const std::vector<std::string>& lines = JSONNumberFactory<requirements, arg1, arg2>::as_json_lines(value);
            
            for(int i = 0; i < int(lines.size()); ++i)
                if(i != lines.size() - 1)
                    result += lines[i] + "\n";
                else
                    result += lines[i];
            
            return result;
        }
    };
    
    enum JSONStringFactoryCapabilities
    {
        UnsetStringCapability = 0,
        MinimumLengthStringCapabilty = 1,
        MaximumLengthStringCapabilty = 2,
        RegexStringCapabilty = 4
    };
    
    
    template <bool has_minimum, long long int minimum>
    class CheckLengthMinimum
    {
    };
    
    template <long long int minimum>
    class CheckLengthMinimum<true, minimum>
    {
    public:
        template <typename T>
        static bool valid(const T& input)
        {
            return input.length() >= minimum;
        }
    };
    
    template <long long int minimum>
    class CheckLengthMinimum<false, minimum>
    {
    public:
        template <typename T>
        static bool valid(const T& input)
        {
            return true;
        }
    };
    
    template <bool has_maximum, long long int maximum>
    class CheckLengthMaximum
    {
    };
    
    template <long long int maximum>
    class CheckLengthMaximum<true, maximum>
    {
    public:
        template <typename T>
        static bool valid(const T& input)
        {
            return input.length() <= maximum;
        }
    };
    
    template <long long int maximum>
    class CheckLengthMaximum<false, maximum>
    {
    public:
        template <typename T>
        static bool valid(const T& input)
        {
            return true;
        }
    };
    
    template <bool has_regex, typename Regex>
    class CheckRegex
    {
    };
    
    template <typename Regex>
    class CheckRegex<true, Regex>
    {
    public:
        const static constexpr std::regex match_regex(typename Regex::name);
        
        template <typename T>
        static bool valid(const T& input)
        {
            return match_regex.regex_match(input.begin(), input.end());
        }
    };
    
    template <typename Regex>
    class CheckRegex<false, Regex>
    {
    public:
        template <typename T>
        static bool valid(const T& input)
        {
            return true;
        }
    };
    
    template <JSONStringFactoryCapabilities requirements,
              long long int min_length = 0,
              long long int max_length = std::numeric_limits<long long int>::max(),
              typename RegexString = CompileStr<>>
    class CheckStringRequirements
    {
    public:
        template <typename T>
        static bool valid(const T& input)
        {
            constexpr bool perform_minimum = requirements & MinimumLengthStringCapabilty;
            constexpr bool perform_maximum = requirements & MaximumLengthStringCapabilty;
            constexpr bool perform_regex = requirements & RegexStringCapabilty;
            
            return CheckLengthMinimum<perform_minimum, min_length>::valid(input) && 
                   CheckLengthMaximum<perform_maximum, max_length>::valid(input) &&
                   CheckRegex<perform_regex, RegexString>::valid(input);
        }
    };
    
    template <JSONStringFactoryCapabilities requirements,
              long long int min_length = 0,
              long long int max_length = std::numeric_limits<long long int>::max(),
              typename RegexString = CompileStr<>>
    class JSONStringFactory
    {
    public:
        typedef std::string ValueType;
        
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
            
            if(!CheckStringRequirements<requirements, min_length, max_length, RegexString>::valid(value))
                throw JSONValidationException();
            return value;
        }
        
        template <typename InputType>
        static ValueType parse(InputType& input)
        {
            InputSource<InputType> source(input);
            return JSONStringFactory<requirements, min_length, max_length, RegexString>::parse_impl(source);
        }
        
        static std::vector<std::string> as_json_lines(const ValueType& value)
        {
            return std::vector<std::string>{escape_string(value)};
        }
        
        static std::string as_json(const ValueType& value)
        {
            std::string result = "";
            const std::vector<std::string>& lines = JSONStringFactory<requirements, min_length, max_length, RegexString>::as_json_lines(value);
            
            for(int i = 0; i < int(lines.size()); ++i)
                result += lines[i] + "\n";
            
            return result;
        }
    };
    
    enum JSONBooleanFactoryCapabilities
    {
        UnsetBooleanCapability = 0
    };
    
    template <JSONBooleanFactoryCapabilities requirements>
    class CheckBooleanRequirements
    {
    public:
        template <typename T>
        static bool valid(const T& input)
        {
            return true;
        }
    };
    
    template <JSONBooleanFactoryCapabilities requirements>
    class JSONBooleanFactory
    {
    public:
        typedef bool ValueType;
        
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
            
            if(!CheckBooleanRequirements<requirements>::valid(value))
                throw JSONValidationException();
            
            return value;
        }
        
        template <typename InputType>
        static ValueType parse(InputType& input)
        {
            InputSource<InputType> source(input);
            return JSONBooleanFactory<requirements>::parse_impl(source);
        }
        
        static std::vector<std::string> as_json_lines(const ValueType& value)
        {
            return std::vector<std::string>{value ? "true" : "false"};
        }
        
        static std::string as_json(const ValueType& value)
        {
            std::string result = "";
            const std::vector<std::string>& lines = JSONBooleanFactory<requirements>::as_json_lines(value);
            
            for(int i = 0; i < int(lines.size()); ++i)
                result += lines[i] + "\n";
            
            return result;
        }
    };
    
    enum JSONArrayFactoryCapabilities
    {
        UnsetArrayCapability = 0,
        MinimumLengthArrayCapability = 1,
        MaximumLengthArrayCapability = 2
    };
    
    template <bool perform, long long int minimum>
    class CheckArrayLengthMinimum
    {
    };
    
    template <long long int minimum>
    class CheckArrayLengthMinimum<true, minimum>
    {
    public:
        template <typename T>
        static bool valid(const T& input)
        {
            return input.size() >= minimum;
        }
    };
    
    template <long long int minimum>
    class CheckArrayLengthMinimum<false, minimum>
    {
    public:
        template <typename T>
        static bool valid(const T& input)
        {
            return true;
        }
    };
    
    template <bool perform, long long int maximum>
    class CheckArrayLengthMaximum
    {
    };
    
    template <long long int maximum>
    class CheckArrayLengthMaximum<true, maximum>
    {
    public:
        template <typename T>
        static bool valid(const T& input)
        {
            return input.size() <= maximum;
        }
    };
    
    template <long long int maximum>
    class CheckArrayLengthMaximum<false, maximum>
    {
    public:
        template <typename T>
        static bool valid(const T& input)
        {
            return true;
        }
    };
    
    template <JSONArrayFactoryCapabilities requirements, long long int minimum, long long int maximum>
    class CheckArrayRequirements
    {
    public:
        template <typename T>
        static bool valid(const T& input)
        {
            constexpr bool perform_minimum = requirements & MinimumLengthArrayCapability;
            constexpr bool perform_maximum = requirements & MaximumLengthArrayCapability;
            
            return CheckArrayLengthMinimum<perform_minimum, minimum>::valid(input) &&
                   CheckArrayLengthMaximum<perform_maximum, maximum>::valid(input);
        }
    };
    
    template <JSONArrayFactoryCapabilities requirements, typename T, 
              long long int min_length = 0, 
              long long int max_length = std::numeric_limits<long long int>::max()>
    class JSONArrayFactory
    {
    public:
        typedef std::vector<typename T::ValueType> ValueType;
        
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
            
            if(!CheckArrayRequirements<requirements, min_length, max_length>::valid(value))
                throw JSONValidationException();
            
            return value;
        }
        
        template <typename InputType>
        static ValueType parse(InputType& input)
        {
            InputSource<InputType> source(input);
            return JSONArrayFactory<requirements, T, min_length, max_length>::parse_impl(source);
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
            const std::vector<std::string>& lines = JSONArrayFactory<requirements, T, min_length, max_length>::as_json_lines(value);
            
            for(int i = 0; i < int(lines.size()); ++i)
                result += lines[i] + "\n";
            
            return result;
        }
    };
    
    template <typename... Args> 
    class JSONSet;
    
    template <> 
    class JSONSet<>
    {
    public:
        template <typename Input>
        static void parse_against_parameter(JSONSet<>& result, std::string parameter_name, Input& input)
        {
            throw BadJSONFormatException("Parameter was not found in structure: " + parameter_name, input.tell());
        }
        
        static void find_missing_parameters(std::vector<std::string>& missing_fields, std::unordered_set<std::string> seen_parameters)
        {
        }
        
        template <typename Param>
        void get(std::string parameter_name, Param& parameter) const 
        {
            throw std::logic_error("Parameter not found: " + parameter_name);
        }
        
        class JSONNoSuchParameter
        {
        };
        
        template <char... chrs>
        class ParameterType
        {
        public:
            typedef JSONNoSuchParameter Type;
        };
        
        void as_json_fields(std::vector<std::string>& output_lines, bool is_first = true) const
        {
        }
        
        template <typename T, char... Chrs>
        void get_into(T& val) const
        {
        }
        
        template <typename T, char... Chrs>
        void set(T& val)
        {
        }
    };
    
    template <typename HeadType, typename... Rest> 
    class JSONSet<HeadType, Rest...>
    {
    public:
        typedef typename HeadType::NameType JSONSetNameType;
        typename HeadType::Type::ValueType item;
        JSONSet<Rest...> rest;
        
        
        template <char... chrs>
        class ParameterType
        {
        public:
            typedef typename std::conditional<std::is_same<CompileStr<chrs...>, typename HeadType::NameType>::value, 
            typename HeadType::Type, 
            typename JSONSet<Rest...>::template ParameterType<chrs...>::Type>::type Type;
        };
        
        template <typename Input>
        static void parse_against_parameter(JSONSet<HeadType, Rest...>& result, std::string parameter_name, Input& input)
        {
            if(parameter_name == HeadType::name)
                result.item = HeadType::Type::parse_impl(input);
            else
                JSONSet<Rest...>::parse_against_parameter(result.rest, parameter_name, input);
        }
        
        static void find_missing_parameters(std::vector<std::string>& missing_fields, std::unordered_set<std::string> seen_parameters)
        {
            if(seen_parameters.count(HeadType::name) == 0)
                missing_fields.push_back(std::string(HeadType::name));
            
            JSONSet<Rest...>::find_missing_parameters(missing_fields, seen_parameters);
        }
        
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
        
        template <typename T, char... Chrs>
        void get_into(T& val) const
        {
            copy_if_true<std::is_same<CompileStr<Chrs...>, typename HeadType::NameType>::value, T, typename HeadType::Type::ValueType>::copy(val, item);
            rest.template get_into<T, Chrs...>(val);
        }
        
        template <char... Chrs>
        typename ParameterType<Chrs...>::Type::ValueType get() const
        {
            typename ParameterType<Chrs...>::Type::ValueType val;
            get_into<typename ParameterType<Chrs...>::Type::ValueType, Chrs...>(val);
            return val;
        }
        
        template <typename T, char... Chrs>
        void set(T& val)
        {
            copy_if_true<std::is_same<CompileStr<Chrs...>, typename HeadType::NameType>::value, 
                         typename HeadType::Type::ValueType, 
                         T>::copy(item, val);
            rest.template set<T, Chrs...>(val);
        }
        
        void as_json_fields(std::vector<std::string>& output_lines, bool is_first = true) const
        {
            if(!is_first)
                output_lines.back() += ",";
            
            std::vector<std::string> values = HeadType::Type::as_json_lines(item);
            std::string field_name = "  \"" + escape_string(HeadType::name) + "\": ";
            std::string first_line = field_name + values[0];
            output_lines.push_back(first_line);
            
            for(int i = 1; i < int(values.size()); ++i)
                output_lines.push_back("  " + values[i]);
            
            rest.as_json_fields(output_lines, false);
        }
    };
    
    enum JSONObjectFactoryCapabilities
    {
        UnsetObjectCapability = 0,
        DoNotAssumeValidObjectCapability = 1
    };
    
    template <bool track_parameter_names>
    class ObjectParameterTracker
    {
    public:
        template <typename Input>
        void add_parameter(const std::string& parameter, Input& input)
        {
        }
        
        template <typename T, typename Input>
        void finish(T& object, Input& input)
        {
        }
    };
    
    template <>
    class ObjectParameterTracker<true>
    {
    public:
        std::unordered_set<std::string> parameters;
        
        template <typename Input>
        void add_parameter(const std::string& parameter, Input& input)
        {
            bool insert_happened = parameters.insert(parameter).second;
            
            if(!insert_happened)
                throw BadJSONFormatException("The following field was duplicated: " + parameter, input.tell()); 
        }
        
        template <typename T, typename Input>
        void finish(T& object, Input& input)
        {
            std::vector<std::string> missing_fields;
            T::find_missing_parameters(missing_fields, parameters);
            
            if(missing_fields.empty())
                return;
            
            std::string fields = "";
            for(int i = 0; i < int(missing_fields.size()) - 1; ++i)
                fields += missing_fields[i] + ", ";
            fields += missing_fields.back();
            
            throw BadJSONFormatException("The following fields were not found in the input: " + fields, input.tell());
        }
    };
    
    template <JSONObjectFactoryCapabilities requirements, typename... ObjectEntries>
    class JSONObjectFactory
    {
    public:
        typedef JSONSet<ObjectEntries...> ValueType;
        
        template <typename Input>
        static ValueType parse_impl(Input& input)
        {
            JSONSet<ObjectEntries...> value;
            constexpr bool check_validity = requirements & DoNotAssumeValidObjectCapability;
            ObjectParameterTracker<check_validity> parameter_tracker;
            
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
                
                std::string field_name = JSONStringFactory<UnsetStringCapability>::parse_impl(input);
                parameter_tracker.add_parameter(field_name, input);
                
                eat_whitespace(input);
                char expected_colon = input.getc();
                eat_whitespace(input);
                
                if(expected_colon != ':')
                    throw BadJSONFormatException("A colon was expected.", input.tell());
                
                JSONSet<ObjectEntries...>::parse_against_parameter(value, field_name, input);
                
                is_first_run = false;
            }
            
            parameter_tracker.finish(value, input);
            
            return value;
        }
        
        template <typename InputType>
        static ValueType parse(InputType& input)
        {
            InputSource<InputType> source(input);
            return JSONObjectFactory<requirements, ObjectEntries...>::parse_impl(source);
        }
        
        static std::vector<std::string> as_json_lines(const ValueType& value)
        {
            std::vector<std::string> result = std::vector<std::string>(1, "{");
            value.as_json_fields(result);
            result.push_back("}");
            
            return result;
        }
        
        static std::string as_json(const ValueType& value)
        {
            std::string result = "";
            const std::vector<std::string>& lines = JSONObjectFactory<requirements, ObjectEntries...>::as_json_lines(value);
            
            for(int i = 0; i < int(lines.size()); ++i)
                result += lines[i] + "\n";
            
            return result;
        }
    };
    
}