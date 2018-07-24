#!/usr/bin/env python

"""
Highlight nonmanifold vertices.
"""

import argparse
import numpy as np
import pymesh

def parse_args():
    parser = argparse.ArgumentParser(description=__doc__);
    parser.add_argument("--debug-output", default=None, help="output debug mesh");
    parser.add_argument("input_mesh", help="input mesh");
    parser.add_argument("output_mesh", help="output mesh");
    return parser.parse_args();

def main():
    args = parse_args();
    mesh = pymesh.load_mesh(args.input_mesh);

    pymesh.is_vertex_manifold(mesh);
    pymesh.is_edge_manifold(mesh);

    faces = mesh.faces;
    edge_manifold = mesh.get_attribute("edge_manifold").reshape((-3,
        mesh.vertex_per_face));
    is_edge_nonmanifold = np.any(edge_manifold==0, axis=1);
    involved_faces = faces[is_edge_nonmanifold];
    mesh.add_attribute("faces_adj_nonmanifold_edge");
    mesh.set_attribute("faces_adj_nonmanifold_edge", is_edge_nonmanifold);

    print("Faces adjacent to nonmanifold edges");
    print("Indices: {}".format(np.arange(mesh.num_faces)[is_edge_nonmanifold]));
    print(involved_faces);
    pymesh.save_matrix("nonmanifold_edges.dmat", involved_faces);

    vertex_manifold = mesh.get_attribute("vertex_manifold").ravel();
    is_vertex_nonmanifold = np.any(vertex_manifold[faces] == 0, axis=1);
    involved_faces = faces[is_vertex_nonmanifold];
    mesh.add_attribute("faces_adj_nonmanifold_vertex");
    mesh.set_attribute("faces_adj_nonmanifold_vertex", is_vertex_nonmanifold);

    print("Faces adjacent to nonmanifold vertices");
    print("Indices: {}".format(np.arange(mesh.num_faces)[is_vertex_nonmanifold]));
    print(involved_faces);
    pymesh.save_matrix("nonmanifold_vertices.dmat", involved_faces);

    pymesh.save_mesh(args.output_mesh, mesh, *mesh.attribute_names);

    if args.debug_output is not None:
        debug_faces = faces[is_edge_nonmanifold];
        debug_mesh = pymesh.form_mesh(mesh.vertices, debug_faces);
        pymesh.save_mesh(args.debug_output, debug_mesh);

if __name__ == "__main__":
    main();
