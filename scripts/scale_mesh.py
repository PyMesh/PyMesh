#!/usr/bin/env python

"""
Scalar input mesh.
"""

import argparse
import pymesh
import numpy as np

def parse_args():
    parser = argparse.ArgumentParser(description=__doc__);
    parser.add_argument("--scale", "-s", type=float, default=1.0,
            help="Uniform scaling factor");
    parser.add_argument("--scale-x", "-x", type=float, default=None,
            help="X axis scaling factor");
    parser.add_argument("--scale-y", "-y", type=float, default=None,
            help="Y axis scaling factor");
    parser.add_argument("--scale-z", "-z", type=float, default=None,
            help="Z axis scaling factor");
    parser.add_argument("input_mesh", help="input mesh");
    parser.add_argument("output_mesh", help="output mesh");
    return parser.parse_args();

def main():
    args = parse_args();
    scale = np.ones(3) * args.scale;
    if args.scale_x is not None:
        scale[0] = args.scale_x;
    if args.scale_y is not None:
        scale[1] = args.scale_y;
    if args.scale_z is not None:
        scale[2] = args.scale_z;

    mesh = pymesh.load_mesh(args.input_mesh);
    mesh = pymesh.form_mesh(mesh.vertices * scale, mesh.faces, mesh.voxels);
    pymesh.save_mesh(args.output_mesh, mesh);

if __name__ == "__main__":
    main();
