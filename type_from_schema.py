import json
import os
import sys

def print_usage():
    print """./type_from_schema <json_schema> [ARGS]
    
Options
 --typedef <type_name>\t\t\t Displays the type as a typedef."""

def type_of_number(obj):
    return "JSONNumber"

type_of_integer = type_of_number

def type_of_string(obj):
    return "JSONString"
    
def type_of_bool(obj):
    return "JSONBool"
    
def type_of_array(obj):
    result = "JSONHomogenousArray<"
    
    sub_func_name = "type_of_" + obj["items"]["type"]
    result += globals()[sub_func_name](obj["items"])
    
    result += ">"
    
    return result

def type_of_object(obj):
    result = "JSONSet<"
    
    properties = obj["properties"]
    
    is_first = True
    
    for prop, val in properties.items():
        sub_func_name = "type_of_" + val["type"]
        if is_first:
            is_first = False
        else:
            result += ", "
        result += "NamedType<" + globals()[sub_func_name](val) + ", str_to_list_{}(\"{}\")".format(len(prop), prop)     
    
    result += ">"
    
    return result

def main(filename, args):
    typedef_name = None
    for i in range(len(args)):
        if args[i] == "--typedef":
            i += 1
            typedef_name = args[i]
            
    with open(filename) as f:
        loaded_json = json.loads(f.read())
    
    if typedef_name is not None:
        sys.stdout.write("typedef ")
        
    sys.stdout.write(type_of_object(loaded_json))
    
    if typedef_name is not None:
        sys.stdout.write(" " + typedef_name + ";")
    
    sys.stdout.write("\n")

if len(sys.argv) == 1:
    print_usage()
else:
    main(sys.argv[1], sys.argv[1:])