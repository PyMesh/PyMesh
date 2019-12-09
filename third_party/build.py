#!/usr/bin/env python

""" Build and install third party dependencies for PyMesh.
"""

import argparse
import subprocess
import os
import os.path
import tempfile
import shutil

def parse_args():
    parser = argparse.ArgumentParser(__doc__);
    parser.add_argument("package", choices=["cgal", "cork", "carve", "eigen",
        "quartet", "tetgen", "triangle", "qhull", "clipper", "geogram"]);
    return parser.parse_args();

def get_pymesh_dir():
    return os.path.join(os.getcwd(), "..");

def build_cgal():
    build_dir = tempfile.mkdtemp();
    pymesh_dir = get_pymesh_dir();

    # Configure cgal
    cmd = "cmake" + \
            " -S {}/third_party/cgal".format(pymesh_dir) + \
            " -B {}".format(build_dir) + \
            " -DWITH_CGAL_ImageIO=OFF" + \
            " -DWITH_CGAL_Qt5=Off" + \
            " -DCMAKE_INSTALL_PREFIX={}/python/pymesh/third_party/".format(pymesh_dir);
    subprocess.check_call(cmd.split());

    # Build cgal
    cmd = "cmake --build {}".format(build_dir);
    subprocess.check_call(cmd.split());

    cmd = "cmake --build {} --target install".format(build_dir);
    subprocess.check_call(cmd.split());

    # Clean up
    shutil.rmtree(build_dir)

def build_generic(libname):
    build_dir = tempfile.mkdtemp();
    pymesh_dir = get_pymesh_dir();

    # Configure cgal
    cmd = "cmake" + \
            " -S {}/third_party/{}".format(pymesh_dir, libname) + \
            " -B {}".format(build_dir) + \
            " -DBUILD_SHARED_LIBS=Off" + \
            " -DCMAKE_INSTALL_PREFIX={}/python/pymesh/third_party/".format(pymesh_dir);
    subprocess.check_call(cmd.split());

    # Build cgal
    cmd = "cmake --build {}".format(build_dir);
    subprocess.check_call(cmd.split());

    cmd = "cmake --build {} --target install".format(build_dir);
    subprocess.check_call(cmd.split());

    # Clean up
    shutil.rmtree(build_dir)

def main():
    args = parse_args();
    if args.package == "cgal":
        build_cgal();
    elif args.package == "clipper":
        build_generic("Clipper/cpp");
    else:
        build_generic(args.package);

if __name__ == "__main__":
    main();
