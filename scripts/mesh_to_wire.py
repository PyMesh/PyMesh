#!/usr/bin/env python

"""
Convert a triangular mesh into a primal or dual wire network.
"""

import argparse
import numpy as np
import pymesh

def mesh_to_graph(mesh):
    mesh.enable_connectivity();
    vertices = mesh.vertices;
    edges = [];
    for vi in range(mesh.num_vertices):
        adj_vertices = mesh.get_vertex_adjacent_vertices(vi);
        for vj in adj_vertices:
            if vj > vi:
                edges.append((vi, vj));
    return vertices, np.array(edges, dtype=int)

def mesh_to_dual_graph(mesh):
    mesh.enable_connectivity();
    mesh.add_attribute("face_centroid");
    vertices = mesh.get_face_attribute("face_centroid");
    edges = [];
    for fi in range(mesh.num_faces):
        adj_faces = mesh.get_face_adjacent_faces(fi);
        for fj in adj_faces:
            if fj > fi:
                edges.append((fi, fj));
    return vertices, np.array(edges, dtype=int);

def parse_args():
    parser = argparse.ArgumentParser(description=__doc__);
    parser.add_argument("--dual", "-d", help="Use the dual graph",
            action="store_true");
    parser.add_argument("mesh_file", help="input mesh file");
    parser.add_argument("wire_file", help="output wire file");
    return parser.parse_args();

def main():
    args = parse_args();
    mesh = pymesh.load_mesh(args.mesh_file);
    if not args.dual:
        vertices, edges = mesh_to_graph(mesh);
    else:
        vertices, edges = mesh_to_dual_graph(mesh);
    wire_network = pymesh.wires.WireNetwork.create_from_data(vertices, edges);
    wire_network.write_to_file(args.wire_file);

if __name__ == "__main__":
    main();
