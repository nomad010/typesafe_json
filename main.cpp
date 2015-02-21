/**
Copyright (c) <2014> <Julian Kenwood>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**/
#include <cstdio>
#include "typesafe_json.h"

/** Example app. **/ 

using namespace TypeSafeJSON;

int main(int argc, char** argv)
{   
    std::string json_string = R"json(
{
  "id": "428bf64f-a828-4f06-a6c2-98a23533093c",
  "list": [1, 2, 3]
})json";
    
    typedef JSONObjectFactory<SillyObjectValidator,
                              NamedType<JSONStringFactory<>, str_to_list_2("id")>, 
                              NamedType<JSONArrayFactory<JSONNumberFactory<long double>, UniqueArrayValidator>, str_to_list_4("list")>> JSONType;
    
    JSONType::ValueType obj;
    try
    {
        obj = JSONType::parse(json_string);
    }
    catch(BadJSONFormatException& exception)
    {
        printf("%s at position %d\n", exception.what(), exception.get_position());
    }
    
    std::string id = obj.get<str_to_list_2("id")>();
    id += "derp";
    obj.set<std::string, str_to_list_2("id")>(id);
    
    printf("%s\n", JSONType::as_json(obj).c_str());

    std::vector<long double> json = obj.get<str_to_list_4("list")>();
    long double sum = 0;
    
    for(int i = 0; i < int(json.size()); ++i)
        sum += json[i];
    printf("%Lf\n", sum); 
}