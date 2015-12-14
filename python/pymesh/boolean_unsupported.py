"""
This module contains python wrapper of boolean libraries that is not officially
supported by PyMesh project, mostly because I either do not have access to the
source code or cannot release the source code.  The following boolean libraries
are included here:

    * QuickCSG: http://kinovis.inrialpes.fr/static/QuickCSG/
"""

import numpy as np
import os
import os.path
from subprocess import check_call
import tempfile
from time import time
from .meshio import save_mesh, load_mesh, form_mesh

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

def quick_csg(mesh_1, mesh_2, operation, with_timing=False):
    exe_name = None;
    for suffix in ["mac", "linux", "win.exe"]:
        exe_name = which("mesh_csg_{}".format(suffix));
        if exe_name is not None:
            break;
    if exe_name is None:
        raise NotImplementedError("Cannot find QuickCSG executable.");

    tmp_dir = tempfile.mkdtemp();
    file_1 = os.path.join(tmp_dir, "mesh1.off");
    file_2 = os.path.join(tmp_dir, "mesh2.off");
    output_file = os.path.join(tmp_dir, "out.off");

    # Use anoymous to avoid QuickCSG parsing errors.
    save_mesh(file_1, mesh_1, anonymous=True);
    save_mesh(file_2, mesh_2, anonymous=True);

    op_flag = {
            "union": "-union",
            "intersection": "-inter",
            "difference": "-diff 1",
            "symmetric_difference": "-xor",
            };
    if operation not in op_flag:
        raise NotImplementedError("Unsupported boolean operation: {}"\
                .format(operation));

    command = "{} -tess3 {} -O {} {} {}".format(
            exe_name, op_flag[operation], output_file, file_1, file_2);
    if with_timing:
        start_time = time();
    check_call(command.split());
    if with_timing:
        finish_time = time();
        running_time = finish_time - start_time;

    assert(os.path.exists(output_file));
    mesh = load_mesh(output_file);

    os.remove(file_1);
    os.remove(file_2);
    os.remove(output_file);
    os.rmdir(tmp_dir);

    if with_timing:
        return mesh, running_time
    else:
        return mesh;
