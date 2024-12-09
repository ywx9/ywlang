import os
import re
import sys
import glob
import json
import subprocess

env_json = {}
# creates ".vscode/environment.json" if not exists
if not os.path.exists(".vscode/environment.json"):
  print("Creating .vscode/environment.json")
  # find file "C:\Program Files*\Microsoft Visual Studio\*\*\VC\Tools\MSVC\*\bin\Hostx64\x64\cl.exe"
  for path in glob.glob(R"C:\Program Files*\Microsoft Visual Studio\*\*\VC\Tools\MSVC\*\bin\Hostx64\x64\cl.exe"):
    if os.path.exists(path):
      env_json["cl_exe"] = path
      break
  if "cl_exe" not in env_json:
    print("cl.exe not found")
    sys.exit(1)
  env_json["msvc_lib"] = env_json["cl_exe"].replace("\\bin\\Hostx64\\x64\\cl.exe", "\\lib\\x64")
  env_json["msvc_inc"] = env_json["cl_exe"].replace("\\bin\\Hostx64\\x64\\cl.exe", "\\include")
  # find directory "C:\Program Files*\Windows Kits\10\Lib\*\ucrt"
  for path in glob.glob(R"C:\Program Files*\Windows Kits\10\Include\*\ucrt"):
    if os.path.exists(path):
      env_json["ucrt_inc"] = path
      break
  if "ucrt_inc" not in env_json:
    print("Windows Kits 10 not found")
    sys.exit(1)
  env_json["ucrt_lib"] = env_json["ucrt_inc"].replace("Include", "Lib") + "\\x64"
  env_json["um_lib"] = env_json["ucrt_lib"].replace("ucrt", "um")
  env_json["um_inc"] = env_json["ucrt_inc"].replace("ucrt", "um")
  env_json["shared_inc"] = env_json["ucrt_inc"].replace("ucrt", "shared")
  env_json["winrt_inc"] = env_json["ucrt_inc"].replace("ucrt", "winrt")
  env_json["cppwinrt_inc"] = env_json["ucrt_inc"].replace("ucrt", "cppwinrt")
  with open(".vscode/environment.json", "w", encoding="utf-8") as f:
    f.write(json.dumps(env_json, indent=2))

# reads ".vscode/environment.json"
with open(".vscode/environment.json", "r", encoding="utf-8") as f:
  env_json = json.load(f)
cl_exe = env_json["cl_exe"]
msvc_lib = env_json["msvc_lib"]
msvc_inc = env_json["msvc_inc"]
ucrt_lib = env_json["ucrt_lib"]
ucrt_inc = env_json["ucrt_inc"]
um_lib = env_json["um_lib"]
um_inc = env_json["um_inc"]
shared_inc = env_json["shared_inc"]
winrt_inc = env_json["winrt_inc"]
cppwinrt_inc = env_json["cppwinrt_inc"]

# compile "*.yw" files
yw_files = glob.glob("*.yw")
for yw_file in yw_files:
  yw = None
  with open(yw_file, "r", encoding="utf-8") as f:
    yw = f.read()
  if yw is None:
    print(f"Error: {yw_file} cannot be read")
    continue
  # remove comments
  yw = re.sub(r"//.*", "", yw)
  yw = re.sub(r"/\*.*?\*/", "", yw, flags=re.DOTALL)
  # replace newlines with spaces
  yw = yw.replace("\n", " ")
  yw = yw.replace("\r", " ")
  # remove multiple spaces
  yw = re.sub(r"\s+", " ", yw)
  # remove leading and trailing spaces
  yw = yw.strip()
  # write the C++ file
  cpp_file = yw_file.replace(".yw", ".cpp")
  with open(cpp_file, "w", encoding="utf-8") as f:
    f.write("#include \"ywlang.hpp\"\nusing namespace yw;\nint main() {\n")
    f.write(yw)
    f.write("\n  return 0;\n}\n")
  # compile the C++ file
  args = [cl_exe, cpp_file, "/std:c++latest", "/EHsc", "/nologo", "/W4", "/O2", "/Qpar", "/utf-8", ]
  args += [f"/I{msvc_inc}", f"/I{ucrt_inc}", f"/I{um_inc}", f"/I{shared_inc}", f"/I{winrt_inc}", f"/I{cppwinrt_inc}", ]
  args += [f"/link", f"/LIBPATH:{msvc_lib}", f"/LIBPATH:{ucrt_lib}", f"/LIBPATH:{um_lib}", ]
  subprocess.run(args)

# def yw2cpp(yw_file: str):
#   # reads the YW file
#   with open(yw_file, "r") as f:
#     yw = f.read()

#   # removes comments
#   yw = re.sub(r"//.*", "", yw)
#   yw = re.sub(r"/\*.*?\*/", "", yw, flags=re.DOTALL)

#   # replaces newlines with spaces
#   yw = yw.replace("\n", " ")
#   yw = yw.replace("\r", " ")

#   # removes multiple spaces
#   yw = re.sub(r"\s+", " ", yw)

#   # removes leading and trailing spaces
#   yw = yw.strip()

#   # converts ""..."" to "U"..."s"
#   yw = re.sub(r""(.*?)"", r"U"\1"s", yw)

#   # writes the C++ file
#   cpp_file = yw_file.replace(".yw", ".cpp")
#   with open(cpp_file, "w", encoding="utf-8") as f:
#     f.write("#include \"ywlang.hpp\"\nusing namespace yw;\nint main() {\n")
#     f.write(yw)
#     f.write("\n  return 0;\n}\n")

#   return cpp_file

# if __name__ == "__main__":
#   if len(sys.argv) != 2:
#     print("Usage: python script.py <yw_file>")
#     sys.exit(1)

#   yw2cpp(sys.argv[1])

#   # compiles the C++ file by calling clang++
#   os.system(f"clang++ -std=c++20 -o {sys.argv[1].replace(".yw", ".exe")} {sys.argv[1].replace(".yw", ".cpp")} -cxx-isystem ")
