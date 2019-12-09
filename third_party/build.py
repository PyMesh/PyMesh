#!/usr/bin/env python

""" Build and install third party dependencies for PyMesh.
"""

import argparse
import subprocess
import os
import os.path
import tempfile
import shutil
import sys

def parse_args():
    parser = argparse.ArgumentParser(__doc__);
    parser.add_argument("package", choices=["cgal", "cork", "carve", "eigen",
        "quartet", "tetgen", "triangle", "qhull", "clipper", "geogram", "draco",
        "tbb"]);
    return parser.parse_args();

def get_pymesh_dir():
    return os.path.join(sys.path[0], "..");

def build_generic(libname, build_flags=""):
    build_dir = tempfile.mkdtemp();
    pymesh_dir = get_pymesh_dir();

    # Configure cgal
    cmd = "cmake" + \
            " -S {}/third_party/{}".format(pymesh_dir, libname) + \
            " -B {}".format(build_dir) + \
            " -DBUILD_SHARED_LIBS=Off" + \
            build_flags + \
            " -DCMAKE_INSTALL_PREFIX={}/python/pymesh/third_party/".format(pymesh_dir);
    subprocess.check_call(cmd.split());

    # Build cgal
    cmd = "cmake --build {}".format(build_dir);
    subprocess.check_call(cmd.split());

    cmd = "cmake --build {} --target install".format(build_dir);
    subprocess.check_call(cmd.split());

    # Clean up
    shutil.rmtree(build_dir)

def build(package):
    if package == "cgal":
        build_generic("cgal", " -DWITH_CGAL_ImageIO=Off -DWITH_CGAL_Qt5=Off");
    elif package == "clipper":
        build_generic("Clipper/cpp");
    elif package == "tbb":
        build_generic("tbb", " -DTBB_BUILD_SHARED=On -DTBB_BUILD_STATIC=Off");
    else:
        build_generic(package);

def main():
    args = parse_args();
    build(args.package);

if __name__ == "__main__":
    main();
