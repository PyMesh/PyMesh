#!/usr/bin/env python

from setuptools import setup
from distutils.command.build import build
from subprocess import check_call;
import os
import os.path

root_dir = os.path.abspath(os.path.dirname(__file__));
package_dir = os.path.join(root_dir, "python/pymesh");

class cmake_build(build):
    def run(self):
        print("Overriding default build process");
        build_dir = os.path.join(root_dir, "build");
        if not os.path.isdir(build_dir):
            os.mkdir(build_dir);

        os.chdir(build_dir);
        command = "cmake ..";
        check_call(command.split());
        command = "make";
        check_call(command.split());
        command = "make tools";
        check_call(command.split());
        os.chdir(root_dir);

        lib_dir = os.path.join(root_dir, "lib");
        package_lib_dir = os.path.join(package_dir, "lib");
        if (os.path.exists(package_lib_dir)):
            os.unlink(package_lib_dir);
        os.symlink(lib_dir, package_lib_dir);

        swig_dir = os.path.join(root_dir, "swig");
        package_swig_dir = os.path.join(package_dir, "swig");
        if (os.path.exists(package_swig_dir)):
            os.unlink(package_swig_dir);
        os.symlink(swig_dir, package_swig_dir);

        build.run(self);

setup(
        name = "pymesh",
        description = "Mesh Processing for Python",
        version = "0.1",
        author = "Qingnan Zhou",
        author_email = "qnzhou@gmail.com",
        license = "MPL",
        package_dir = {"": "python"},
        packages = ["pymesh", "pymesh.misc", "pymesh.meshutils"],
        package_data = {"pymesh": ["swig/*.py", "lib/*.so", "lib/*.dylib", "lib/*.dll"]},
        include_package_data = True,
        cmdclass={'build': cmake_build},
        scripts=[
            "scripts/add_index.py",
            "scripts/box_gen.py",
            "scripts/curvature.py",
            "scripts/fix_mesh.py",
            "scripts/merge.py",
            "scripts/outer_hull.py",
            "scripts/point_cloud.py",
            ],
        );
