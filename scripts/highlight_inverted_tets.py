#!/usr/bin/env python

"""
Highlight inverted tets.
"""

import argparse
import numpy as np
import pymesh

def parse_args():
    parser = argparse.ArgumentParser(description=__doc__);
    parser.add_argument("input_mesh", help="input mesh");
    parser.add_argument("output_mesh", help="output mesh");
    return parser.parse_args();

def main():
    args = parse_args();
    mesh = pymesh.load_mesh(args.input_mesh);

    vertices = mesh.vertices;
    voxels = mesh.voxels;
    num_voxels = mesh.num_voxels;
    orientation = np.zeros(num_voxels);

    for i in range(num_voxels):
        tet = voxels[i];
        orientation[i] = pymesh.orient_3D(
                vertices[tet[1]],
                vertices[tet[0]],
                vertices[tet[2]],
                vertices[tet[3]]);
        if orientation[i] < 0:
            orientation[i] = -1;
        elif orientation[i] > 0:
            orientation[i] = 1;

    mesh.add_attribute("orientation");
    mesh.set_attribute("orientation", orientation);

    pymesh.save_mesh(args.output_mesh, mesh, "orientation");

if __name__ == "__main__":
    main();
