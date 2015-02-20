typesafe_json
=============

A header-only typesafe library for handling JSON from C++. This library requires a relatively recent compiler with good C++11 support.

The structure of the JSON is defined by the type: 

* JSONNumberFactory for a numeric entry. The output type defaults to long double. At the moment only long doubles and int will really work.
* JSONStringFactory for a UTF8 textual entry. The output type defaults to std::string, other types should support += and .push_back
* JSONBooleanFactory for a boolean entry. The output type defaults to bool, other types should support assignment of bool values.
* JSONArrayFactory for arrays of items of type T. Heterogenous lists of items are not supported. The output type defaults to std::vector<T>, but other types should support push_back.
* JSONObjectFactory for sets of types. The types must be of class NamedType<typename T, char... Chrs> where T is the actual JSON Type Factory and Chrs represents the name of the parameter. There is a handy macro str_to_list_X(where X is the size up to 32) for converting strings to lists of chars for the template. Parameters may appear in any order when parsed. The result is a JSONSet where the required parameters can be retrieved with get,

All these types appear in the TypeSafeJSON namespace. Each type factory has a validator, to mimic JSONSchemaValidators that can be overridden. Note: all factories except JSONObjectFactory have the last parameter as template template parameter which defaults to a default validator that does no validation. The JSONObjectFactory accepts a validator as the first (non-defaulting) parameter due to its variadic parameters.

This library is intended for safety, not speed. At present it is not overly slow. Just don't expect it to have similar performance in the future.

At the moment only parsing from FILE* and std::string is supported. Use Type::parse(FILE* fptr) or Type::parse(std::string text) to parse the structure and ObjFactory::as_json(obj) to get a prettified string. New input sources can be added by implementing getc(like ::getc), ungetc(like ::ungetc), get_value<T>(using ::scanf) and tell(like ::tell).

Planned features:

* Heterogenous lists.
* Better Unicode support.
* C++03 support. This was my intention from the get go, but the implementation of JSONSet put a stop to that.
* More built-in validators.