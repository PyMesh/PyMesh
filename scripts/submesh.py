#!/usr/bin/env python

"""
Extract a subset of faces of the input mesh.
"""

import argparse
import numpy as np
import pymesh

def expand_by_one_ring(mesh, selected_faces):
    selected_vertices = np.zeros(mesh.num_vertices, dtype=bool);
    selected_vertices[mesh.faces[selected_faces].ravel()] = True;
    selected_faces = np.any(selected_vertices[mesh.faces], axis=1);
    return selected_faces;

def extract_submesh(mesh, face_indices, n_ring):
    vertices = mesh.vertices;

    selected_faces = np.zeros(mesh.num_faces, dtype=bool);
    selected_faces[face_indices] = True;
    ring_index = np.zeros(mesh.num_faces, dtype=int);
    ring_index[selected_faces] += 1;
    for i in range(n_ring):
        selected_faces = expand_by_one_ring(mesh, selected_faces);
        ring_index[selected_faces] += 1;
    selected_face_indices = np.arange(mesh.num_faces)[selected_faces];
    faces = mesh.faces[selected_faces];
    ring_index = (ring_index[selected_faces] - n_ring - 1) * (-1);

    vertices, faces, __ = pymesh.remove_isolated_vertices_raw(vertices, faces);
    mesh = pymesh.form_mesh(vertices, faces);
    mesh.add_attribute("ori_face_index");
    mesh.set_attribute("ori_face_index", selected_face_indices);
    mesh.add_attribute("ring");
    mesh.set_attribute("ring", ring_index);
    return mesh;

def parse_args():
    parser = argparse.ArgumentParser(description=__doc__);
    parser.add_argument("--n-ring", "-n", type=int, default=0,
            help="Expand the selection by n-rings");
    parser.add_argument("input_mesh", help="input mesh");
    parser.add_argument("output_mesh", help="output mesh");
    parser.add_argument("face_indices", nargs="+",
            type=int, help="indices of faces to be extracted");
    return parser.parse_args();

def main():
    args = parse_args();
    mesh = pymesh.load_mesh(args.input_mesh);
    mesh = extract_submesh(mesh, args.face_indices, args.n_ring);
    pymesh.save_mesh(args.output_mesh, mesh, *mesh.get_attribute_names());

if __name__ == "__main__":
    main();
