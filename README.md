typesafe_json
=============

A header-only typesafe library for handling JSON from C++. This library requires a relatively recent compiler with good C++11 support.

The structure of the JSON is defined by the type: 

* JSONNumberFactory for a numeric entry. The output type defaults to long double. At the moment only long doubles and int will really work.
* JSONStringFactory for a UTF8 textual entry. The output type defaults to std::string, other types should support += and .push_back
* JSONBooleanFactory for a boolean entry. The output type defaults to bool, other types should support assignment of bool values.
* JSONArrayFactory for arrays of items of type T. Note this T refers to the factory type. Heterogenous lists of items are not supported. The output type defaults to std::vector<T>, but other types should support push_back.
* JSONObjectFactory for named sets of types. This is handled be the JSONSet which is a workaround for not having introspectable classes. Note that this class is complicated, and as such is probably a bit kludgy. The types for JSONSet must be of class NamedType<typename T, char... Chrs> where T is the actual JSON Type Factory and Chrs represents the name of the parameter. It would be great if c++ accepted string constants as parameters, but for now there is a handy macro str_to_list_X(where X is the size up to 32) for converting strings to lists of chars for the template. JSONObject parameters may appear in any order when parsed, but it is an error if they are duplicated or missing. The result is a JSONSet where the required parameters can be retrieved with the get member function template.

All these types appear in the TypeSafeJSON namespace. Each type factory can be passed a validator, to mimic JSONSchemaValidators that can be overridden. All factories have the last parameter as template template parameter which defaults to a default validator that does no validation. This makes it easier to write validators as JSONSet typenames can grow to be complex.

This library is intended for safety, not speed. At present it is not noticeably slow. Although if you have very complicated json objects expect longer compile times as a result. Don't expect it to have similar performance in the future.

At the moment only parsing from FILE* and std::string is supported. Use Type::parse(FILE* fptr) or Type::parse(std::string text) to parse the structure and ObjFactory::as_json(obj) to get a prettified string. New input sources can be added by implementing getc(like ::getc), ungetc(like ::ungetc), get_value<T>(using ::scanf) and tell(like ::tell).

Planned features:

* Heterogenous lists.
* Better Unicode support.
* C++03 support. This was my intention from the get go, but the implementation of JSONSet put a stop to that.
* More built-in validators.
* Some benchmarks.
* Some form of class support.
