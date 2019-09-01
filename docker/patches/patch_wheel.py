#!/usr/bin/env python

import zipfile
import os
import os.path
from subprocess import check_output, check_call
import re
import tempfile

dist_dir = "/root/PyMesh/dist"
tmp_dir = tempfile.gettempdir();
pymesh_lib_pattern = re.compile("libPyMesh-.*\.so");

for f in os.listdir(dist_dir):
    __, ext = os.path.splitext(f);
    if (ext != ".whl"):
        continue;
    wheel_file = os.path.join(dist_dir, f);
    extraction_dir = os.path.join(tmp_dir, "tmp_wheel");
    cmd = "unzip -q -o {} -d {}".format(wheel_file, extraction_dir);
    check_call(cmd.split());

    cmd = "mv {} {}.old".format(wheel_file, wheel_file);
    check_call(cmd.split());

    lib_dir = os.path.join(extraction_dir, "pymesh/lib");
    for lib_file in os.listdir(lib_dir):
        if pymesh_lib_pattern.match(lib_file) is None:
            continue;
        cmd = "./package_dependencies.py {}".format(
                os.path.join(lib_dir, lib_file));
        check_call(cmd.split(), cwd="/root/PyMesh/docker/patches");

    cmd = "rm -rf {}".format(os.path.join(extraction_dir, "pymesh/third_party"));
    check_call(cmd.split());

    contents = [];
    for item in os.listdir(extraction_dir):
        contents.append(item);
    cmd = "zip -q -r {} {}".format(wheel_file, " ".join(contents));
    check_call(cmd.split(), cwd=extraction_dir);

    cmd = "rm -rf {}".format(extraction_dir);
    check_call(cmd.split());

