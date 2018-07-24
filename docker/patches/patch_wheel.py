#!/usr/bin/env python

import zipfile
import os
import os.path

dependencies = [
        "/usr/lib/x86_64-linux-gnu/libboost_system.so.1.62.0",
        "/usr/lib/x86_64-linux-gnu/libboost_thread.so.1.62.0",
        "/usr/lib/x86_64-linux-gnu/libboost_chrono.so.1.62.0",
        "/usr/lib/x86_64-linux-gnu/libboost_date_time.so.1.62.0",
        "/usr/lib/x86_64-linux-gnu/libboost_atomic.so.1.62.0",
        "/usr/lib/x86_64-linux-gnu/libgmp.so.10",
        "/usr/lib/x86_64-linux-gnu/libmpfr.so.6",
        "/usr/lib/x86_64-linux-gnu/libgomp.so.1",
        "/usr/lib/x86_64-linux-gnu/libtbb.so.2",
        "/usr/lib/x86_64-linux-gnu/libtbbmalloc.so.2",
        ];

dist_dir = "/root/PyMesh/dist"
for f in os.listdir(dist_dir):
    wheelfile = os.path.join(dist_dir, f);
    zfout = zipfile.ZipFile(wheelfile, 'a')
    for dependency in dependencies:
        assert(os.path.exists(dependency));
        basename = os.path.basename(dependency);
        zfout.write(dependency, os.path.join("pymesh/lib/", basename));
