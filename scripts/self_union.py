#!/usr/bin/env python

"""
Resolve self-intersection and nested shells using self-union.
"""

import argparse
import numpy as np
import pymesh

def parse_args():
    parser = argparse.ArgumentParser(description=__doc__);
    parser.add_argument("input_mesh", help="Input mesh");
    parser.add_argument("output_mesh", help="Output mesh");
    return parser.parse_args();

def main():
    args = parse_args();
    mesh = pymesh.load_mesh(args.input_mesh);
    mesh, __ = pymesh.remove_degenerated_triangles(mesh, 100);
    empty_mesh = pymesh.form_mesh(
            np.zeros((0, mesh.dim)),
            np.zeros((0, mesh.vertex_per_face)));
    result = pymesh.boolean(mesh, empty_mesh, "union", engine="igl");
    #result, __ = pymesh.remove_degenerated_triangles(result, 100);
    pymesh.save_mesh(args.output_mesh, result);

if __name__ == "__main__":
    main();
