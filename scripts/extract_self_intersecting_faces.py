#!/usr/bin/env python

"""
Extract self-intersecting faces.
"""

import argparse
import numpy as np
import pymesh

def extract_intersecting_faces(mesh, selection):
    face_pairs = pymesh.detect_self_intersection(mesh);
    selected_faces = np.zeros(mesh.num_faces, dtype=bool);

    if selection is not None:
        selected_pairs = np.any(face_pairs == selection, axis=1);
        face_pairs = face_pairs[selected_pairs];

    selected_faces[face_pairs[:,0]] = True;
    selected_faces[face_pairs[:,1]] = True;
    faces = mesh.faces[selected_faces];
    intersecting_mesh = pymesh.form_mesh(mesh.vertices, faces);
    intersecting_mesh, __ = pymesh.remove_isolated_vertices(intersecting_mesh);
    return intersecting_mesh;

def parse_args():
    parser = argparse.ArgumentParser(description=__doc__);
    parser.add_argument("--selection", "-s", required=False, type=int,
            default=None, help="Extract faces intersecting the selection");
    parser.add_argument("input_mesh");
    parser.add_argument("output_mesh");
    return parser.parse_args();

def main():
    args = parse_args();
    mesh = pymesh.load_mesh(args.input_mesh);
    intersecting_mesh = extract_intersecting_faces(mesh, args.selection);
    pymesh.save_mesh(args.output_mesh, intersecting_mesh);

if __name__ == "__main__":
    main();

