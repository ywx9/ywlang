#ident size: 2
#encoding: utf-8

import os
import sys
import json
import subprocess

env_json = {}
# reads .vscode/environment.json
if not os.path.exists('.vscode/environment.json'):
  print("Please run ywstd.py first")
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

# compiles ywlib as module
if os.path.exists('ywlib.ifc'):
  os.remove('ywlib.ifc')
args = [cl_exe, "/c", "ywlib.ixx", "/std:c++latest", "/EHsc", "/nologo", "/W4", "/O2", "/Qpar", "/utf-8", "/DYWLIB_COMPILE=true", "/DYWSTD_IMPORT=true", ]
args += ["/Foywlib.obj", "/ifcOutputywlib.ifc", ]
args += [f"/I{msvc_inc}", f"/I{ucrt_inc}", f"/I{um_inc}", f"/I{shared_inc}", f"/I{winrt_inc}", f"/I{cppwinrt_inc}", ]
args += [f"/reference ywstd=ywstd.ifc", ]
print("starts compiling ywlib")
subprocess.run(args, check=True)
if os.path.exists('ywlib.ifc'):
  print("ywlib successfully compiled")
else:
  print("failed to compile ywlib")
  sys.exit(1)
