#!/usr/bin/env python

""" This script package all system dependencies into a single folder and reset
the rpath using `patchelf`.  (Linux only)
"""

import argparse
import os.path
import platform
import re
from subprocess import check_output, check_call

def parse_args():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument("--dependency-dir", default=None,
            help="Directory where external dependencies are collected")
    parser.add_argument("lib_file")
    return parser.parse_args()

def extract_rpaths_darwin(lib_file):
    # MacOS only.
    cmd = "otool -l {}".format(lib_file)
    output = check_output(cmd.split()).decode("ASCII")

    pattern = "cmd LC_RPATH\n\s*cmdsize\s+\d+\n\s*path\s+(.*)\s+\(offset\s+\d+\)"
    rpaths = re.findall(pattern, output, re.M)
    return rpaths

def extract_rpaths(lib_file):
    system_name = platform.system().lower()
    if system_name == "linux":
        return []
    elif system_name == "darwin":
        return extract_rpaths_darwin(lib_file)
    else:
        raise NotImplementedError(
                "Platform {} is not supported.".format(system_name))

def get_dependencies_posix(lib_file, dep_dir):
    cmd = "ldd {}".format(lib_file)
    dep_libs = check_output(cmd.split()).decode("ASCII")

    dependencies = []
    for lib in dep_libs.split("\n"):
        fields = lib.split()
        if len(fields) >= 3:
            libname = fields[2]
        else:
            continue
        libname = libname.strip()
        path,name = os.path.split(libname)
        dep_lib = os.path.join(dep_dir, name)

        if not os.path.exists(dep_lib):
            dependencies.append(libname)

    return dependencies

def get_dependencies_darwin(lib_file, dep_dir):
    cmd = "otool -L {}".format(lib_file)
    dep_libs = check_output(cmd.split()).decode("ASCII")

    dependencies = []
    for lib in dep_libs.split("\n"):
        fields = lib.split()
        if len(fields) >= 2:
            libname = fields[0]
        else:
            continue
        libname = libname.strip()
        path,name = os.path.split(libname)
        dep_lib = os.path.join(dep_dir, name)

        if not os.path.exists(dep_lib):
            dependencies.append(libname)

    return dependencies

def get_dependencies(lib_file, dep_dir):
    system_name = platform.system().lower()
    if system_name == "linux":
        return get_dependencies_posix(lib_file, dep_dir)
    elif system_name == "darwin":
        return get_dependencies_darwin(lib_file, dep_dir)
    else:
        raise NotImplementedError(
                "Platform {} is not supported.".format(system_name))

def migrate_dependency_posix(lib_file, dep_file, dep_dir, rpaths):
    dep_lib_name = os.path.basename(dep_file)
    cmd = "cp {} {}".format(dep_file, os.path.join(dep_dir, dep_lib_name))
    check_call(cmd.split())

    cmd = "patchelf --add-needed {} {}".format(dep_lib_name, lib_file)
    check_call(cmd.split())

def migrate_dependency_darwin(lib_file, dep_file, dep_dir, rpaths):
    if dep_file.startswith("@rpath"):
        name = "/".join(dep_file.split("/")[1:])
        # Need to search rpath
        for rpath in rpaths:
            if not os.path.isdir(rpath):
                continue
            fullpath = os.path.join(rpath, name)
            if os.path.exists(fullpath):
                dep_file = fullpath
                break
    else:
        assert(os.path.exists(dep_file))
        fullpath = dep_file

    dep_lib_name = os.path.basename(dep_file)
    cmd = "cp {} {}".format(dep_file, os.path.join(dep_dir, dep_lib_name))
    check_call(cmd.split())

    cmd = "install_name_tool -change {} {} {}".format(dep_file,
            "@rpath/{}".format(dep_lib_name), lib_file)
    check_call(cmd.split())

def migrate_dependency(lib_file, dep_file, dep_dir, rpaths):
    system_name = platform.system().lower()
    if system_name == "linux":
        return migrate_dependency_posix(lib_file, dep_file, dep_dir, rpaths)
    elif system_name == "darwin":
        return migrate_dependency_darwin(lib_file, dep_file, dep_dir, rpaths)
    else:
        raise NotImplementedError(
                "Platform {} is not supported.".format(system_name))

def reset_rpath_posix(lib_file, dep_dir, rpaths):
    lib_dir, __ = os.path.split(lib_file)
    rel_dep_dir = os.path.relpath(dep_dir, lib_dir)
    cmd = "patchelf --set-rpath $ORIGIN:$ORIGIN/{} {}".format(
            rel_dep_dir, lib_file)
    check_call(cmd.split())

def reset_rpath_darwin(lib_file, dep_dir, rpaths):
    for rpath in rpaths:
        cmd = "install_name_tool -delete_rpath {} {}".format(rpath, lib_file)
        check_call(cmd.split())

    lib_dir, __ = os.path.split(lib_file)
    rel_dep_dir = os.path.relpath(dep_dir, lib_dir)
    cmd = "install_name_tool -add_rpath @loader_path -add_rpath @loader_path/{} {}".format(
            rel_dep_dir, lib_file)
    check_call(cmd.split())

def reset_rpath(lib_file, dep_dir, rpaths):
    system_name = platform.system().lower()
    if system_name == "linux":
        return reset_rpath_posix(lib_file, dep_dir, rpaths)
    elif system_name == "darwin":
        return reset_rpath_darwin(lib_file, dep_dir, rpaths)
    else:
        raise NotImplementedError(
                "Platform {} is not supported.".format(system_name))

def main():
    args = parse_args()
    lib_file = os.path.abspath(args.lib_file)
    lib_path, lib_name = os.path.split(lib_file)
    if args.dependency_dir is None:
        dep_path = lib_path
    else:
        dep_apth = args.dependency_path

    rpaths = extract_rpaths(lib_file)
    dependencies = get_dependencies(lib_file, dep_path)
    for dep in dependencies:
        migrate_dependency(lib_file, dep, dep_path, rpaths)

    reset_rpath(lib_file, dep_path, rpaths)

if __name__ == "__main__":
    main()
