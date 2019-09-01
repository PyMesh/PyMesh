#!/usr/bin/env python

""" This script package all system dependencies into a single folder and reset
the rpath using `patchelf`.  (Linux only)
"""

import argparse
import os.path
import re
from subprocess import check_output, check_call

def parse_args():
    parser = argparse.ArgumentParser(__doc__);
    parser.add_argument("--dependency-dir", default=None,
            help="Directory where external dependencies are collected");
    parser.add_argument("lib_file");
    return parser.parse_args();

def get_dependencies(lib_file, dep_dir):
    cmd = "ldd {}".format(lib_file);
    dep_libs = check_output(cmd.split()).decode("ASCII");

    dependencies = [];
    for lib in dep_libs.split("\n"):
        fields = lib.split();
        if len(fields) >= 3:
            libname = fields[2];
        else:
            continue;
        libname = libname.strip();
        path,name = os.path.split(libname);
        dep_lib = os.path.join(dep_dir, name);

        if not os.path.exists(dep_lib):
            dependencies.append(libname);

    return dependencies;

def migrate_dependency(lib_file, dep_file, dep_dir):
    dep_lib_name = os.path.basename(dep_file);
    cmd = "cp {} {}".format(dep_file, os.path.join(dep_dir, dep_lib_name));
    check_call(cmd.split());

    cmd = "patchelf --add-needed {} {}".format(dep_lib_name, lib_file);
    check_call(cmd.split());

def reset_rpath(lib_file, dep_dir):
    lib_dir, __ = os.path.split(lib_file);
    rel_dep_dir = os.path.relpath(dep_dir, lib_dir);
    cmd = "patchelf --set-rpath $ORIGIN:$ORIGIN/{} {}".format(
            rel_dep_dir, lib_file);
    check_call(cmd.split());

def main():
    args = parse_args();
    lib_file = os.path.abspath(args.lib_file);
    lib_path, lib_name = os.path.split(lib_file);
    if args.dependency_dir is None:
        dep_path = lib_path;
    else:
        dep_apth = args.dependency_path;

    dependencies = get_dependencies(lib_file, dep_path);
    for dep in dependencies:
        migrate_dependency(lib_file, dep, dep_path);

    reset_rpath(lib_file, dep_path);

if __name__ == "__main__":
    main();
