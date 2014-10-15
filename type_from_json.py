import json
import os
import sys

def print_usage():
    print """./type_from_json <json_file> [ARGS]
    
Options
 --typedef <type_name>\t\t\t Displays the type as a typedef."""

def type_of_int(obj):
    return "JSONNumber"

type_of_float = type_of_int

def type_of_string(obj):
    return "JSONString"
    
type_of_unicode = type_of_string

def type_of_bool(obj):
    return "JSONBool"
    
class EmptyListException(Exception):
    def __init__(self):
        super(EmptyListException, self).__init__("Unable to decompose empty list into types")
    
class HeterogenousListException(Exception):
    def __init__(self):
        super(HeterogenousListException, self).__init__("All the types of list elements must be identical.")
    
def type_of_list(li):
    result = "JSONHomogenousArray<"
    
    sub_types = []
    for item in li:
        sub_func_name = "type_of_" + str(item.__class__.__name__)
        sub_type = globals()[sub_func_name](li)
        sub_types.append(sub_type)
        
    if len(sub_types) == 0:
        raise EmptyListException()
    
    if not all(map(lambda x: x == sub_types[0], sub_types)):
        raise HeterogenousListException()
    
    result += sub_types[0]
    result += ">"
    
    return result

def type_of_dict(obj):
    result = "JSONSet<"
    
    is_first = True
    
    for prop, val in obj.items():
        sub_func_name = "type_of_" + str(val.__class__.__name__)
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
        
    sys.stdout.write(type_of_dict(loaded_json))
    
    if typedef_name is not None:
        sys.stdout.write(" " + typedef_name + ";")
    
    sys.stdout.write("\n")

if len(sys.argv) == 1:
    print_usage()
else:
    main(sys.argv[1], sys.argv[1:])