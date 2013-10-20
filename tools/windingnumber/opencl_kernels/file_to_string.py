#!/usr/bin/env python

import os
import os.path

header = """
#pragma once
#include <string>
namespace Kernel {
"""

footer = """
}
"""

def main():
    with open("program.h", 'w') as fout:
        fout.write(header);

        for filename in os.listdir("./"):
            basename, ext = os.path.splitext(filename);
            if ext != ".cl":
                continue;
            with open(filename, 'r') as fin:
                contents = fin.read();
                contents = contents.replace("\n", "\\n")
                fout.write("std::string {}(\"{}\");\n".format(
                    basename, contents));
                fout.write("\n");
        fout.write(footer);


if __name__ == "__main__":
    main();
