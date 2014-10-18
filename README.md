typesafe_json
=============

A header-only typesafe library for handling JSON from C++. This library requires a relatively recent compiler with good C++11 support.

The structure of the JSON is defined by the type: 

* JSONNumberFactory for a numeric entry. The output type is long double.
* JSONStringFactory for a textual entry. The output type is std::string, a UTF-8 encoding of the input string.
* JSONBooleanFactory for a boolean entry. The output type is bool.
* JSONArrayFactory<TFactory> for arrays of items of type T. Heterogenous lists of items are not supported. The output type is std::vector<T>
* JSONObjectFactory<types> for sets of types. The types must be of class NamedType<typename T, char... Chrs> where T is the actual JSON Type Factory and Chrs represents the name of the parameter. There is a handy macro str_to_list_X(where X is the size up to 32) for converting strings to lists of chars for the template. Parameters may appear in any order when parsed. The result is a JSONSet where the required parameters can be retrieved with get,

All these types appear in the TypeSafeJSON namespace.

This library is intended for safety, not speed. At present it is not overly slow. Just don't expect it to have similar performance in the future.

At the moment only parsing from FILE* is permitted, but that will hopefully be rectified soon. Use Type::parse(FILE* fptr) to parse the structure and ObjFactory::as_json(obj) to get a prettified string.

Planned features:

* Heterogenous lists.
* Better Unicode support.
* C++03 support. This was my intention from the get go, but the implementation of JSONSet put a stop to that.
