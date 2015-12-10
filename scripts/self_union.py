#!/usr/bin/env python

"""
Resolve self-intersection and nested shells using self-union.
"""

import argparse
import numpy as np
import pymesh
import logging
import os
import os.path
from subprocess import check_call
import tempfile

def which(program):
    """ Determine the full path of an executable.

    Copied from http://stackoverflow.com/questions/377017/test-if-executable-exists-in-python/377028#377028
    """
    def is_exe(fpath):
        return os.path.isfile(fpath) and os.access(fpath, os.X_OK)

    fpath, fname = os.path.split(program)
    if fpath:
        if is_exe(program):
            return program
    else:
        for path in os.environ["PATH"].split(os.pathsep):
            path = path.strip('"')
            exe_file = os.path.join(path, program)
            if is_exe(exe_file):
                return exe_file

    return None

def run_quick_csg(mesh1, mesh2):
    """ A wrapper to execute QuickCSG binary.
    """
    exe_name = None;
    for suffix in ["mac", "linux", "win.exe"]:
        exe_name = which("mesh_csg_{}".format(suffix));
        if exe_name is not None:
            break;
    assert(exe_name is not None);

    tmp_dir = tempfile.mkdtemp();
    file1 = os.path.join(tmp_dir, "mesh1.off");
    file2 = os.path.join(tmp_dir, "mesh2.off");
    output_file = os.path.join(tmp_dir, "out.off");

    # Use anoymous to avoid QuickCSG parsing errors.
    pymesh.save_mesh(file1, mesh1, anonymous=True);
    pymesh.save_mesh(file2, mesh2, anonymous=True);
    command = "{} -tess3 -inter -O {} {} {}".format(
            exe_name, output_file, file1, file2);
    print(command);
    check_call(command.split());
    assert(os.path.exists(output_file));
    mesh = pymesh.load_mesh(output_file);

    os.remove(file1);
    os.remove(file2);
    os.remove(output_file);
    os.rmdir(tmp_dir);
    return mesh;

def parse_args():
    parser = argparse.ArgumentParser(description=__doc__);
    parser.add_argument("--engine",
            choices=["igl", "carve", "cgal", "cork", "corefinement",
                "quick_csg"],
            default="igl", help="Boolean engine to use");
    parser.add_argument("input_mesh", help="Input mesh");
    parser.add_argument("output_mesh", help="Output mesh");
    return parser.parse_args();

def main():
    args = parse_args();
    mesh = pymesh.load_mesh(args.input_mesh);
    if mesh.vertex_per_face == 4:
        logging.warning("Converting quad mesh to triangle mesh.");
        mesh = pymesh.quad_to_tri(mesh);

    bbox = mesh.bbox;
    center = (bbox[0] + bbox[1]) * 0.5;
    box = pymesh.generate_box_mesh(
            (bbox[0]-center)*1.1 + center,
            (bbox[1]-center)*1.1 + center);
    if args.engine == "quick_csg":
        result = run_quick_csg(mesh, box);
    else:
        result = pymesh.boolean(
                mesh, box, "intersection", engine=args.engine);
    pymesh.save_mesh(args.output_mesh, result);

if __name__ == "__main__":
    main();
