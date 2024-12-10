# indent size: 2
# encoding: utf-8

import os
import re
import sys
import glob
import json
import subprocess

env_json = {}
# reads .vscode/environment.json
if not os.path.exists('.vscode/environment.json'):
  print("Please run ywstd.py and ywlib.py first")
  sys.exit(1)
with open('.vscode/environment.json', 'r', encoding='utf-8') as f:
  env_json = json.load(f)
cl_exe = env_json["cl_exe"]
msvc_inc = env_json["msvc_inc"]
msvc_lib = env_json["msvc_lib"]
ucrt_inc = env_json["ucrt_inc"]
ucrt_lib = env_json["ucrt_lib"]
um_inc = env_json["um_inc"]
um_lib = env_json["um_lib"]
shared_inc = env_json["shared_inc"]
winrt_inc = env_json["winrt_inc"]
cppwinrt_inc = env_json["cppwinrt_inc"]

# checks ywstd.ifc and ywstd.obj exist
if not os.path.exists('ywstd.ifc') or not os.path.exists('ywstd.obj'):
  print("Please run ywstd.py first")
  sys.exit(1)

# checks ywlib.ifc and ywlib.obj exist
if not os.path.exists('ywlib.ifc') or not os.path.exists('ywlib.obj'):
  print("Please run ywlib.py first")
  sys.exit(1)

# translates .yw file to .cpp file
yw_file = None
for file in glob.glob("*.yw"):
  yw_file = file
  break
if yw_file is None:
  print("*.yw file not found")
  sys.exit(1)
cpp_file = yw_file.replace(".yw", ".cpp")
if os.path.exists(cpp_file):
  os.remove(cpp_file)
exe_file = yw_file.replace(".yw", ".exe")
if os.path.exists(exe_file):
  os.remove(exe_file)

yw = None
with open(yw_file, "r", encoding="utf-8") as f:
  yw = f.read()
if yw is None:
  print(f"Error: {yw_file} cannot be read")
  sys.exit(1)

# convert "..." to literal_string("...")
# yw = re.sub(r"\"(.*?)\"", r'literal_string("\1")', yw)

with open(cpp_file, "w", encoding="utf-8") as f:
  f.write("#include \"ywstd.hpp\"\n")
  f.write("#include \"ywlib.hpp\"\n")
  f.write("using namespace yw;\n")
  f.write("#define nat size_t\n")
  f.write("#define fat double\n")
  f.write(yw)
  f.write("\nint main() {}\n")

# compile the C++ file
args = [cl_exe, cpp_file, "/std:c++latest", "/EHsc", "/nologo", "/W4", "/O2", "/Qpar", "/utf-8", "/DYWLIB_IMPORT=true", "/DYWSTD_IMPORT=true", ]
# args += [f"/I{msvc_inc}", f"/I{ucrt_inc}", f"/I{um_inc}", f"/I{shared_inc}", f"/I{winrt_inc}", f"/I{cppwinrt_inc}", ]
args += ["/reference ywstd=ywstd.ifc", "/reference ywlib=ywlib.ifc", "ywstd.obj", "ywlib.obj", f"/link /LIBPATH:{msvc_lib} /LIBPATH:{ucrt_lib} /LIBPATH:{um_lib}", ]
subprocess.run(args)
obj_file = yw_file.replace(".yw", ".obj")
if os.path.exists(obj_file):
  os.remove(obj_file)

if "--run" in sys.argv:
  if os.path.exists(exe_file):
    os.system(exe_file)
  else:
    print(f"Error: {exe_file} not found")
    sys.exit(1)


#   f.write("#define fat double\n")
#   f.write("#include <format>\n")
#   f.write("int main() {\n")
#   f.write(yw)
#   f.write("\nreturn 0;\n}\n")
# # compile the C++ file
# args = [cl_exe, cpp_file, "/std:c++latest", "/EHsc", "/nologo", "/W4", "/O2", "/Qpar", "/utf-8", ]
# args += [f"/I{msvc_inc}", f"/I{ucrt_inc}", f"/I{um_inc}", f"/I{shared_inc}", f"/I{winrt_inc}", f"/I{cppwinrt_inc}", ]
# args += [f"/reference ywlang=ywlang.ifc", f"/link ywlang.obj", f"/LIBPATH:{msvc_lib}", f"/LIBPATH:{ucrt_lib}", f"/LIBPATH:{um_lib}", ]
# subprocess.run(args)
# # remove .obj
# obj_file = yw_file.replace(".yw", ".obj")
# if os.path.exists(obj_file):
#   os.remove(obj_file)

# # run the program if "--run" is specified
# if run_program:
#   if os.path.exists(exe_file):
#     os.system(exe_file)
#   else:
#     print(f"Error: {exe_file} not found")
#     sys.exit(1)
