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
#include "variadic_string.h"
#include "inherits_from.h"

namespace TypeSafeJSON
{
    static bool is_valid_unicode(int codepoint)
    {
        return !(codepoint < 0x00000000 || codepoint > 0x0010FFFF);
    }

    static std::string escape_string(const std::string& input)
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
    
    static int hex_to_dec(char hex)
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
    static void eat_whitespace(Input& input)
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
    
    static std::string codepoint_to_utf8(int codepoint)
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
        
        int tell()
        {
            return pos;
        }
    };
    
    class JSONNumber : public InheritsFrom<long double>
    {
    public:
        std::vector<std::string> as_json_lines()
        {
            return std::vector<std::string>{std::to_string((long double)*this)};
        }
        
        std::string as_json()
        {
            std::string result = "";
            const std::vector<std::string>& lines = as_json_lines();
            
            for(int i = 0; i < int(lines.size()); ++i)
                result += lines[i] + "\n";
            
            return result;
        }
        
        template <typename Input>
        static JSONNumber parse_impl(Input& input)
        {
            JSONNumber result;
            result.value = input.get_number();
            return result;
        }
        
        template <typename InputType>
        static JSONNumber parse(InputType& input)
        {
            InputSource<InputType> source(input);
            return JSONNumber::parse_impl(source);
        }
        
        static std::string get_schema()
        {
            return "{\n  \"type\": \"number\"\n}";
        }
        
    };
    
    
    
    class JSONString : public std::string
    {
    public:
        std::vector<std::string> as_json_lines()
        {
            return std::vector<std::string>{escape_string(*this)};
        }
        
        std::string as_json()
        {
            std::string result = "";
            const std::vector<std::string>& lines = as_json_lines();
            
            for(int i = 0; i < int(lines.size()); ++i)
                result += lines[i] + "\n";
            
            return result;
        }
        
        template <typename Input>
        static JSONString parse_impl(Input& input)
        {
            int expected_double_quote = input.getc();
            
            if(expected_double_quote != '"')
                throw BadJSONFormatException("Expected opening double quote", input.tell());
            JSONString result;
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
                        result.push_back('"');
                    }
                    else if(next_chr == '\\')
                    {
                        result.push_back('\\');
                    }
                    else if(next_chr == '/')
                    {
                        result.push_back('/');
                    }
                    else if(next_chr == 'b')
                    {
                        result.push_back('\b');
                    }
                    else if(next_chr == 'f')
                    {
                        result.push_back('\f');
                    }
                    else if(next_chr == 'n')
                    {
                        result.push_back('\n');
                    }
                    else if(next_chr == 'r')
                    {
                        result.push_back('\r');
                    }
                    else if(next_chr == 't')
                    {
                        result.push_back('\t');
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
                        result += chrs;
                    }
                    else
                    {
                        result.push_back('\\');
                        input.ungetc('\\');
                    }
                }
                else if(chr == '"')
                {
                    break;
                }
                else
                {
                    result.push_back(chr);
                }
                
            }
            
            return result;
        }
        
        template <typename InputType>
        static JSONString parse(InputType& input)
        {
            InputSource<FILE*> source(input);
            return JSONString::parse_impl(source);
        }
        
        static std::string get_schema()
        {
            return "{\n  \"type\": \"string\"\n}";
        }
    };
    
    class JSONNull
    {
    public:
        std::vector<std::string> as_json_lines()
        {
            return std::vector<std::string>{"null"};
        }
        
        std::string as_json()
        {
            std::string result = "";
            const std::vector<std::string>& lines = as_json_lines();
            
            for(int i = 0; i < int(lines.size()); ++i)
                result += lines[i] + "\n";
            
            return result;
        }
        
        template <typename Input>
        static JSONNull parse_impl(Input& input)
        {
            int expected_n =  input.getc();
            if(expected_n != 'n')
                throw BadJSONFormatException("Unrecognised null", input.tell());
            
            int expected_u =  input.getc();
            if(expected_u != 'u')
                throw BadJSONFormatException("Unrecognised null", input.tell());
            
            int expected_l1 =  input.getc();
            if(expected_l1 != 'l')
                throw BadJSONFormatException("Unrecognised null", input.tell());
            
            int expected_l2 =  input.getc();
            if(expected_l2 != 'l')
                throw BadJSONFormatException("Unrecognised null", input.tell());
            
            return JSONNull();
        }
        
        template <typename InputType>
        static JSONNull parse(InputType& input)
        {
            InputSource<InputType> source(input);
            return JSONNull::parse_impl(source);
        }
        
        static std::string get_schema()
        {
            return "";
        }
    };
    
    class JSONBool : public InheritsFrom<bool>
    {
    public:
        std::vector<std::string> as_json_lines()
        {
            return std::vector<std::string>{bool(*this) ? "true" : "false"};
        }
        
        std::string as_json()
        {
            std::string result = "";
            const std::vector<std::string>& lines = as_json_lines();
            
            for(int i = 0; i < int(lines.size()); ++i)
                result += lines[i] + "\n";
            
            return result;
        }
        
        template <typename Input>
        static JSONBool parse_impl(Input& input)
        {
            JSONBool result;
            
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
                
                result.value = true;
                return result;
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
                
                result.value = false;
                return result;
            }
            else
            {
                throw BadJSONFormatException("Unrecognised boolean", input.tell());
            }
        }
        
        template <typename InputType>
        static JSONBool parse(InputType& input)
        {
            InputSource<InputType> source(input);
            return JSONBool::parse_impl(source);
        }
        
        static std::string get_schema()
        {
            return "{\n  \"type\": \"boolean\"\n}";
        }
    };
    
    template <typename T>
    class JSONHomogenousArray : public std::vector<T>
    {
    public:
        std::vector<std::string> as_json_lines()
        {
            if(std::vector<T>::empty()) 
            {
                return std::vector<std::string>{"[]"};
            }
            else
            {
                std::vector<std::string> result{"["};
                
                for(int i = 0; i < int(std::vector<T>::size()); ++i)
                {
                    const std::vector<std::string>& rep = std::vector<T>::data()[i].as_json_lines();
                    for(int j = 0; j < int(rep.size()); ++j)
                        result.push_back("  " + rep[j]);
                    
                    if(i != int(std::vector<T>::size()) - 1)
                        result.back() += ",";
                }
                result.push_back("]");
                return result;
            }
        }
        
        std::string as_json()
        {
            std::string result = "";
            const std::vector<std::string>& lines = as_json_lines();
            
            for(int i = 0; i < int(lines.size()); ++i)
                result += lines[i] + "\n";
            
            return result;
        }
        
        template <typename Input>
        static JSONHomogenousArray<T> parse_impl(Input& input)
        {
            JSONHomogenousArray<T> result;
            
            int expected_bracket = input.getc();
            
            if(expected_bracket != '[')
                throw BadJSONFormatException("Expected opening brace", input.tell());
            eat_whitespace(input);
            
            while(true)
            {
                T entry = T::parse_impl(input);
                
                result.push_back(entry);
                
                int is_comma_or_close_bracket = input.getc();
                
                if(is_comma_or_close_bracket == ']')
                    break;
                else if(is_comma_or_close_bracket != ',')
                    throw BadJSONFormatException("Expected comma or closing bracket", input.tell());
                
                eat_whitespace(input);
            }
            
            return result;
        }
        
        template <typename InputType>
        static JSONHomogenousArray<T> parse(InputType& input)
        {
            InputSource<InputType> source(input);
            return JSONHomogenousArray<T>::parse_impl(source);
        }
        
        static std::string get_schema()
        {
            return "{\"type\": \"array\", \"items\":" + T::get_schema() + "\n}";
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
        
        template <typename Input>
        static JSONSet<> parse_impl(Input& input)
        {
            return JSONSet<>();
        }
        
        template <typename Param>
        void get(std::string parameter_name, Param& parameter)
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
        
        void as_json_fields(std::vector<std::string>& output_lines, bool is_first = true)
        {
        }
        
        std::vector<std::string> as_json_lines()
        {
            return std::vector<std::string>(1, "{}");
        }
        
        std::string as_json()
        {
            std::string result = "";
            const std::vector<std::string>& lines = as_json_lines();
            
            for(int i = 0; i < int(lines.size()); ++i)
                result += lines[i] + "\n";
            
            return result;
        }
        
        template <typename T, char... Chrs>
        void get_into(T& val)
        {
        }
        
        template <typename InputType>
        static JSONSet<> parse(InputType& input)
        {
            InputSource<InputType> source(input);
            return JSONSet<>::parse_impl(source);
        }
        
        static std::string get_properties(bool is_first = true)
        {
            return "";
        }
        
        static std::string get_schema()
        {
            return "{\"type\": \"object\", \"properties\": {} }";
        }
    };
    
    template <typename HeadType, typename... Rest> 
    class JSONSet<HeadType, Rest...>
    {
    public:
        typedef typename HeadType::NameType JSONSetNameType;
        typename HeadType::Type item;
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
        
        template <typename Input>
        static JSONSet<HeadType, Rest...> parse_impl(Input& input)
        {
            JSONSet<HeadType, Rest...> result;
            std::unordered_set<std::string> seen_parameters;
            
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
                
                JSONString field_name = JSONString::parse_impl(input);
                
                if(seen_parameters.count(field_name) == 1)
                    throw BadJSONFormatException("The following field was duplicated: " + field_name, input.tell());
                seen_parameters.insert(field_name);
                
                eat_whitespace(input);
                char expected_colon = input.getc();
                eat_whitespace(input);
                
                if(expected_colon != ':')
                    throw BadJSONFormatException("A colon was expected.", input.tell());
                
                JSONSet<HeadType, Rest...>::parse_against_parameter(result, field_name, input);
                
                is_first_run = false;
            }
            
            if(sizeof...(Rest) + 1 != seen_parameters.size())
            {
                std::vector<std::string> missing_fields;
                JSONSet<HeadType, Rest...>::find_missing_parameters(missing_fields, seen_parameters);
                
                std::string fields = "";
                for(int i = 0; i < int(missing_fields.size()) - 1; ++i)
                    fields += missing_fields[i] + ", ";
                fields += missing_fields.back();
                
                throw BadJSONFormatException("The following fields were not found in the input: " + fields, input.tell());
            }
            
            return result;
        }

        template<bool truth, typename T, typename U> 
        class copy_if_true
        {
        };
        
        template<typename T, typename U> 
        class copy_if_true<true, T, U>
        {
        public:
            static void copy(T& output, U& input)
            {
                output = input;
            }
        };
        
        template<typename T, typename U> 
        class copy_if_true<false, T, U>
        {
        public:
            static void copy(T& output, U& input)
            {
            }
        };
        
        template <typename T, char... Chrs>
        void get_into(T& val)
        {
            copy_if_true<std::is_same<CompileStr<Chrs...>, typename HeadType::NameType>::value, T, typename HeadType::Type>::copy(val, item);
            rest.template get_into<T, Chrs...>(val);
        }
        
        template <char... Chrs>
        typename ParameterType<Chrs...>::Type get()
        {
            typename ParameterType<Chrs...>::Type val;
            get_into<typename ParameterType<Chrs...>::Type, Chrs...>(val);
            return val;
        }
        
        void as_json_fields(std::vector<std::string>& output_lines, bool is_first = true)
        {
            if(!is_first)
                output_lines.back() += ",";
            
            std::vector<std::string> values = item.as_json_lines();
            std::string field_name = "  \"" + escape_string(HeadType::name) + "\": ";
            std::string first_line = field_name + values[0];
            output_lines.push_back(first_line);
            
            for(int i = 1; i < int(values.size()); ++i)
                output_lines.push_back("  " + values[i]);
            
            rest.as_json_fields(output_lines, false);
        }
        
        
        std::vector<std::string> as_json_lines()
        {
            std::vector<std::string> result = std::vector<std::string>(1, "{");
            as_json_fields(result);
            result.push_back("}");
            
            return result;
        }
        
        std::string as_json()
        {
            std::string result = "";
            const std::vector<std::string>& lines = as_json_lines();
            
            for(int i = 0; i < int(lines.size()); ++i)
                result += lines[i] + "\n";
            
            return result;
        }
        
        template <typename InputType>
        static JSONSet<HeadType, Rest...> parse(InputType& input)
        {
            InputSource<InputType> source(input);
            return JSONSet<HeadType, Rest...>::parse_impl(source);
        }
        
        static std::string get_properties(bool is_first = true)
        {
            std::string result = "";
            if(!is_first)
                result += ", ";
            result += "\"" + HeadType::name + "\": " + HeadType::Type::get_schema() + JSONSet<Rest...>::get_properties(false);
            return result;
        }
        
        static std::string get_schema()
        {
            return "{\"type\": \"object\", \"properties\": " + JSONSet<HeadType, Rest...>::get_properties(true) + "}";
        }
    };
}