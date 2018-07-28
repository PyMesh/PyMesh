#!/usr/bin/env python

import zipfile
import os
import os.path
from subprocess import check_call

TBB_2018_US_URL = "https://github.com/01org/tbb/releases/download/2018_U5/tbb2018_20180618oss_lin.tgz";
cmd = "wget -O tbb.tgz {}".format(TBB_2018_US_URL);
check_call(cmd.split());

cmd = "tar -zxf tbb.tgz";
check_call(cmd.split());

tbb_libs = [
        "tbb2018_20180618oss/lib/intel64/gcc4.7/libtbb.so.2",
        "tbb2018_20180618oss/lib/intel64/gcc4.7/libtbbmalloc.so.2",
        ];

dependencies = [
        "/usr/lib/x86_64-linux-gnu/libboost_system.so.1.62.0",
        "/usr/lib/x86_64-linux-gnu/libboost_thread.so.1.62.0",
        "/usr/lib/x86_64-linux-gnu/libboost_chrono.so.1.62.0",
        "/usr/lib/x86_64-linux-gnu/libboost_date_time.so.1.62.0",
        "/usr/lib/x86_64-linux-gnu/libboost_atomic.so.1.62.0",
        "/usr/lib/x86_64-linux-gnu/libgmp.so.10",
        "/usr/lib/x86_64-linux-gnu/libmpfr.so.6",
        "/usr/lib/x86_64-linux-gnu/libgomp.so.1",
        ];
dependencies += tbb_libs;

dist_dir = "/root/PyMesh/dist"
for f in os.listdir(dist_dir):
    wheelfile = os.path.join(dist_dir, f);
    zfout = zipfile.ZipFile(wheelfile, 'a')
    for dependency in dependencies:
        assert(os.path.exists(dependency));
        basename = os.path.basename(dependency);
        zfout.write(dependency, os.path.join("pymesh/lib/", basename));

cmd = "rm -rf tbb2018_20180618oss pstl2018_20180618oss tbb.tgz";
check_call(cmd.split());
