#!/usr/bin/env python

""" Pack all shared library dependencies into the wheel itself.
"""

import argparse
import zipfile
import os
import os.path
from subprocess import check_output, check_call
import re
import tempfile

def parse_args():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument("wheels", help="The target wheel file.", nargs="+")
    return parser.parse_args()

def patch_wheel(wheel_file):
    wheel_dir, wheel_filename = os.path.split(wheel_file)
    __, ext = os.path.splitext(wheel_file)
    if (ext != ".whl"):
        return

    # Extract wheel into tmp dir.
    tmp_dir = tempfile.gettempdir()
    extraction_dir = os.path.join(tmp_dir, "tmp_wheel")
    cmd = "unzip -q -o {} -d {}".format(wheel_file, extraction_dir)
    check_call(cmd.split())

    # Back up the original wheel.
    cmd = "mv {} {}.old".format(wheel_file, wheel_file)
    check_call(cmd.split())

    # Gather all dependent shared libs into lib_dir.
    lib_dir = os.path.join(extraction_dir, "pymesh/lib")
    pymesh_lib_pattern = re.compile("libPyMesh-.*\.so")
    for lib_file in os.listdir(lib_dir):
        if pymesh_lib_pattern.match(lib_file) is None:
            continue
        cmd = "./package_dependencies.py {}".format(
                os.path.join(lib_dir, lib_file))
        check_call(cmd.split(), cwd="/root/PyMesh/docker/patches")

    cmd = "rm -rf {}".format(os.path.join(extraction_dir, "pymesh/third_party"))
    check_call(cmd.split())

    # Generate a new wheel.
    contents = []
    for item in os.listdir(extraction_dir):
        contents.append(item)
    cmd = "zip -q -r {} {}".format(wheel_file, " ".join(contents))
    check_call(cmd.split(), cwd=extraction_dir)

    # Clean up.
    cmd = "rm -rf {}".format(extraction_dir)
    check_call(cmd.split())

def main():
    args = parse_args()
    for wheel_file in args.wheels:
        assert(os.path.exists(wheel_file))
        patch_wheel(os.path.abspath(wheel_file))

if __name__ == "__main__":
    main()
