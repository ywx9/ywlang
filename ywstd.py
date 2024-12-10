# indent size: 2
# encoding: utf-8

import os
import sys
import glob
import json
import subprocess

env_json = {}
# creates .vscode/environment.json if not exists
if not os.path.exists('.vscode/environment.json'):
  print("Creating .vscode/environment.json")
  # search MSVC
  cl_exe_path = R"C:\Program Files*\Microsoft Visual Studio\*\*\VC\Tools\MSVC\*\bin\Hostx64\x64\cl.exe"
  for path in glob.glob(cl_exe_path):
    env_json["cl_exe"] = path
    break
  if "cl_exe" not in env_json:
    print("cl.exe not found")
    sys.exit(1)
  env_json["msvc_inc"] = env_json["cl_exe"].replace(R"bin\Hostx64\x64\cl.exe", "include")
  env_json["msvc_lib"] = env_json["msvc_inc"].replace("include", R"\lib\x64")
  # search Windows Kits
  ucrt_inc_path = R"C:\Program Files*\Windows Kits\*\Include\*\ucrt"
  for path in glob.glob(ucrt_inc_path):
    env_json["ucrt_inc"] = path
    break
  if "ucrt_inc" not in env_json:
    print("Windows Kits not found")
    sys.exit(1)
  env_json["ucrt_lib"] = env_json["ucrt_inc"].replace("Include", "Lib") + R"\x64"
  env_json["um_inc"] = env_json["ucrt_inc"].replace("ucrt", "um")
  env_json["um_lib"] = env_json["ucrt_lib"].replace("ucrt", "um")
  env_json["shared_inc"] = env_json["ucrt_inc"].replace("ucrt", "shared")
  env_json["winrt_inc"] = env_json["ucrt_inc"].replace("ucrt", "winrt")
  env_json["cppwinrt_inc"] = env_json["ucrt_inc"].replace("ucrt", "cppwinrt")
  with open('.vscode/environment.json', 'w', encoding='utf-8') as f:
    json.dump(env_json, f, ensure_ascii=False, indent=2)

# reads .vscode/environment.json
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

# compiles ywstd as module
if os.path.exists('ywstd.ifc'):
  os.remove('ywstd.ifc')
args = [cl_exe, "/c", "ywstd.ixx", "/std:c++latest", "/EHsc", "/nologo", "/W4", "/O2", "/Qpar", "/utf-8", "/DYWSTD_COMPILE=true", ]
args += ["/Foywstd.obj", "/ifcOutputywstd.ifc", ]
args += [f"/I{msvc_inc}", f"/I{ucrt_inc}", f"/I{um_inc}", f"/I{shared_inc}", f"/I{winrt_inc}", f"/I{cppwinrt_inc}", ]
print("starts compiling ywstd")
subprocess.run(args, check=True)
if os.path.exists('ywstd.ifc'):
  print("ywstd successfully compiled")
else:
  print("failed to compile ywstd")
  sys.exit(1)
