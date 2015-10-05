#!/usr/bin/env python

"""
Compute the signed volume of an input surface mesh.
"""

import argparse
import numpy as np
import pymesh

def parse_args():
    parser = argparse.ArgumentParser(description=__doc__);
    parser.add_argument("input_mesh", help="input mesh");
    return parser.parse_args();

def main():
    args = parse_args();
    mesh = pymesh.load_mesh(args.input_mesh);
    assert(mesh.dim == 3);

    total_vol = 0.0;
    origin = np.zeros(3);
    vertices = mesh.vertices;
    for f in mesh.faces:
        corners = vertices[f];
        volume = np.dot(np.cross(corners[0], corners[1]), corners[2]);
        total_vol += volume;

    total_vol /= 6.0;
    print("Total volume: {}".format(total_vol));

if __name__ == "__main__":
    main();
