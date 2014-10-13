typesafe_json
=============

A header-only typesafe library for handling JSON from C++. This library requires a relatively recent compiler with good C++11 support.

The structure of the JSON is defined by the type: 

* JSONNumber for a numeric entry. The type is convertible too type long double.
* JSONString for a textual entry. The type inherits from type std::string, a UTF-8 encoding of the input string.
* JSONNull for an entry that is null. Note this a type, items that are unexpectedly null will throw a parse error.
* JSONBool for a boolean entry.
* JSONHomogenousArray<T> for arrays of items of type T. Heterogenous lists of items are not supported.
* JSONSet<types> for sets of types. The types must be of class NamedType<typename T, char... Chrs> where T is the actual JSON Type and Chrs represents the name of the parameter. There is a handy macro str_to_list_X(where X is the size up to 32) for converting strings to lists of chars for the template. Parameters may appear in any order when parsed.

All these types appear in the TypeSafeJSON namespace.

This library is intended for safety, not speed.

At the moment only parsing from FILE* is permitted, but that will hopefully be rectified soon. Use Type::parse(FILE* fptr) to parse the structure and obj.as_json() to get a prettified string.

Planned features:

* Better support for more types for JSON Schema including schema output.
* Heterogenous lists.
* Better Unicode support.
* C++03 support. This was my intention from the get go, but the implementation of JSONSet put a stop to that.
