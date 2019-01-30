#!/usr/bin/env python

import zipfile
import os
import os.path
from subprocess import check_output
import re

dependencies = [];
pymesh_lib_pattern = re.compile("libPyMesh-.*\.so");
lib_dir = "/root/PyMesh/python/pymesh/lib"
for f in os.listdir(lib_dir):
    if pymesh_lib_pattern.match(f) is None:
        continue;
    libfile = os.path.join(lib_dir, f);
    cmd = "ldd {}".format(libfile);
    dep_libs = check_output(cmd.split()).decode('ASCII');
    for lib in dep_libs.split("\n"):
        fields = lib.split();
        if len(fields) >= 3:
            libname = fields[2];
        else:
            continue;
        libname = libname.strip();
        if libname[:4] == "/usr":
            dependencies.append(libname);
dependencies = set(dependencies);

print("The following external dependencies are collected:");
for d in dependencies:
    print("    {}".format(d));

dist_dir = "/root/PyMesh/dist"
for f in os.listdir(dist_dir):
    wheelfile = os.path.join(dist_dir, f);
    zfout = zipfile.ZipFile(wheelfile, 'a')
    for dependency in dependencies:
        assert(os.path.exists(dependency));
        basename = os.path.basename(dependency);
        zfout.write(dependency, os.path.join("pymesh/lib/", basename));

