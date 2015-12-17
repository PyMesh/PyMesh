#!/usr/bin/env python

"""
Highlight non-oriented edges.  A non-oriented edge has inconsistent normals in
its adjacent faces.
"""

import argparse
import numpy as np
import pymesh

def parse_args():
    parser = argparse.ArgumentParser(description=__doc__);
    parser.add_argument("input_mesh");
    parser.add_argument("output_mesh");
    return parser.parse_args();

def main():
    args = parse_args();
    mesh = pymesh.load_mesh(args.input_mesh);
    edge_faces = {};
    for f in mesh.faces:
        edges = [
                sorted([f[0], f[1]]),
                sorted([f[1], f[2]]),
                sorted([f[2], f[0]]),
                ];
        for e in edges:
            if tuple(e) in edge_faces:
                edge_faces[tuple(e)].append(f);
            else:
                edge_faces[tuple(e)] = [f];

    non_oriented_vertices = np.zeros(mesh.num_vertices);
    for e,faces in edge_faces.items():
        if e[0] == e[1]:
            non_oriented_vertices[e[0]] = 1;
            non_oriented_vertices[e[1]] = 1;
            continue;

        count = 0;
        for f in faces:
            if e[0] == f[0] and e[1] == f[1]:
                count+=1;
            if e[0] == f[1] and e[1] == f[2]:
                count+=1;
            if e[0] == f[2] and e[1] == f[0]:
                count+=1;
            if e[1] == f[0] and e[0] == f[1]:
                count-=1;
            if e[1] == f[1] and e[0] == f[2]:
                count-=1;
            if e[1] == f[2] and e[0] == f[0]:
                count-=1;
        if count != 0:
            non_oriented_vertices[e[0]] = 1;
            non_oriented_vertices[e[1]] = 1;

    mesh.add_attribute("non_oriented");
    mesh.set_attribute("non_oriented", non_oriented_vertices);
    pymesh.save_mesh(args.output_mesh, mesh, "non_oriented");

if __name__ == "__main__":
    main();

