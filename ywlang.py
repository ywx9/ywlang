import os
import re
import sys

def yw2cpp(yw_file: str):
  # reads the YW file
  with open(yw_file, 'r') as f:
    yw = f.read()
  
  # removes comments
  yw = re.sub(r'//.*', '', yw)
  yw = re.sub(r'/\*.*?\*/', '', yw, flags=re.DOTALL)

  # replaces newlines with spaces
  yw = yw.replace('\n', ' ')
  yw = yw.replace('\r', ' ')

  # removes multiple spaces
  yw = re.sub(r'\s+', ' ', yw)

  # removes leading and trailing spaces
  yw = yw.strip()

  # converts `"..."` to `U"..."s`
  yw = re.sub(r'"(.*?)"', r'U"\1"s', yw)

  # writes the C++ file
  cpp_file = yw_file.replace('.yw', '.cpp')
  with open(cpp_file, 'w', encoding='utf-8') as f:
    f.write("#include \"ywlang.hpp\"\nusing namespace yw;\nint main() {\n")
    f.write(yw)
    f.write("\n  return 0;\n}\n")

  return cpp_file

if __name__ == '__main__':
  if len(sys.argv) != 2:
    print('Usage: python script.py <yw_file>')
    sys.exit(1)
  
  yw2cpp(sys.argv[1])

  # compiles the C++ file by calling clang++
  os.system(f"clang++ -std=c++20 -o {sys.argv[1].replace('.yw', '.exe')} {sys.argv[1].replace('.yw', '.cpp')}")
