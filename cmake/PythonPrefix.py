import os
import sys

root_dir = os.path.dirname(os.path.dirname(os.path.realpath(__file__)));
with open(os.path.join(root_dir, "cmake/PythonPrefix.cmake"), 'w') as fout:
    fout.write("SET(PYTHON_PREFIX \"{}\")\n".format(sys.prefix));
