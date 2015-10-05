#!/usr/bin/env python

import argparse
import numpy as np
import pymesh

"""
Highlight self-intersecting faces.
"""

def parse_args():
    parser = argparse.ArgumentParser(description=__doc__);
    parser.add_argument("input_mesh", help="input mesh");
    parser.add_argument("output_mesh", help="output mesh");
    return parser.parse_args();

def main():
    args = parse_args();
    mesh = pymesh.load_mesh(args.input_mesh);

    intersecting_faces = pymesh.detect_self_intersection(mesh);
    intersection_marker = np.zeros(mesh.num_faces);
    for i,face_pair in enumerate(intersecting_faces):
        intersection_marker[face_pair] = i+1;

    mesh.add_attribute("intersecting_faces");
    mesh.set_attribute("intersecting_faces", intersection_marker);
    pymesh.save_mesh(args.output_mesh, mesh, "intersecting_faces");

if __name__ == "__main__":
    main();
