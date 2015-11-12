#!/usr/bin/env python

"""
Rigid transform a mesh.
"""

import argparse
import pymesh
import numpy as np
import math

def parse_args():
    parser = argparse.ArgumentParser(description=__doc__);
    parser.add_argument("--offset", "-o", nargs=3, type=float,
            help="offset vector", default=[0, 0, 0]);
    parser.add_argument("--rotation-axis", nargs=3, type=float,
            help="rotation axis", default=[1, 0, 0]);
    parser.add_argument("--rotation-angle", type=float, default=0,
            help="rotation angle");
    parser.add_argument("input_mesh", help="input mesh");
    parser.add_argument("output_mesh", help="output mesh");
    return parser.parse_args();

def main():
    args = parse_args();
    mesh = pymesh.load_mesh(args.input_mesh);
    offset = np.array(args.offset);
    axis = np.array(args.rotation_axis);
    angle = math.radians(args.rotation_angle);
    rot = pymesh.Quaternion.fromAxisAngle(axis, angle);
    rot = rot.to_matrix();

    vertices = mesh.vertices;
    bbox = mesh.bbox;
    centroid = 0.5 * (bbox[0] + bbox[1]);
    vertices = np.dot(rot, (vertices - centroid).T).T + centroid + offset;

    pymesh.save_mesh_raw(args.output_mesh, vertices, mesh.faces, mesh.voxels);

if __name__ == "__main__":
    main();
