import os

print("---Generating Lua exports and header file")
warning = '''// THIS FILE IS AUTOGENERATED BY METAPROGRAMMING.PY

'''

old_cwd = os.getcwd()
os.chdir("C:/Users/dboon/programming/tdengine")

# Generate the header of signatures for functions bound to Lua
# Collect a list of function names
lua_exports_source = None
with open("src/lua_exports_impl.hpp", "r") as f:
    lua_exports_source = f.readlines()

lua_exports_header = open("src/lua_exports.hpp", "w")
lua_exports_header.write(warning)

function_names = []
for line in lua_exports_source:
    if "tdapi" in line:
        # Signature is everything up to the closing paren
        signature = line.split(")")[0]
        signature += ");\n"
        lua_exports_header.write(signature)

        # Now parse the name of the function
        words = line.split(" ")
        name = words[2] # Skip 'tdapi' 'ret_type'
        name = name.split("(") # FN name is everything before arguments
        name = name[0]
        function_names.append(name)
        

lua_exports_header.write("\n")
signature = "void bind_functions();\n"
lua_exports_header.write(signature)

lua_exports_header.close()


# Generate the function to bind functions
bind_file = open("src/bind_functions.hpp", "w")
bind_file.write(warning)

signature = "void bind_functions() {\n\tauto& state = Lua.state;\n"
bind_file.write(signature)
for fn in function_names:
    bind_call = "\t"
    bind_call += "state.set_function("
    bind_call += "\"" + fn + "\""
    bind_call += ", &" + fn
    bind_call += ");\n"
    bind_file.write(bind_call)

bind_file.write("}")

os.chdir(old_cwd)