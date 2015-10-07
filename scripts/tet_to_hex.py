#!/usr/bin/env python

"""
A naive algorithm to convert tet mesh into hex mesh.
"""

import argparse
import numpy as np
import pymesh

def tet_to_hex(mesh):
    in_vertices = mesh.vertices;
    in_voxels = mesh.voxels;
    out_vertices = [];
    out_voxels = [];

    hexes = np.array([
        [0, 6, 12, 4, 5, 11, 14, 13],
        [4, 12, 8, 1, 13, 14, 10, 7],
        [6, 3, 8, 12, 11, 9, 10, 14],
        [5, 2, 9, 11, 13, 7, 10, 14]
        ]);
    for i,tet in enumerate(in_voxels):
        corners = in_vertices[tet];
        e01 = (corners[0] + corners[1]) / 2.0;
        e02 = (corners[0] + corners[2]) / 2.0;
        e03 = (corners[0] + corners[3]) / 2.0;
        e12 = (corners[1] + corners[2]) / 2.0;
        e13 = (corners[3] + corners[1]) / 2.0;
        e23 = (corners[2] + corners[3]) / 2.0;
        f0 = (corners[1] + corners[2] + corners[3]) / 3.0;
        f1 = (corners[0] + corners[2] + corners[3]) / 3.0;
        f2 = (corners[0] + corners[1] + corners[3]) / 3.0;
        f3 = (corners[0] + corners[1] + corners[2]) / 3.0;
        center = np.mean(corners, axis=0).reshape((1, -1));
        out_vertices += [corners,
                e01, e02, e03, e12, e13, e23,
                f0, f1, f2, f3,
                center];

        out_voxels.append(hexes + i*15);

    out_vertices = np.vstack(out_vertices);
    out_voxels = np.vstack(out_voxels);
    out_vertices, out_voxels, __ = pymesh.remove_duplicated_vertices_raw(
            out_vertices, out_voxels);
    mesh = pymesh.form_mesh(out_vertices, None, out_voxels);
    return mesh;

def parse_args():
    parser = argparse.ArgumentParser(description=__doc__);
    parser.add_argument("input_mesh", help="input mesh");
    parser.add_argument("output_mesh", help="output mesh");
    return parser.parse_args();

def main():
    args = parse_args();
    mesh = pymesh.load_mesh(args.input_mesh);
    if (mesh.num_voxels <= 0):
        raise RuntimeError("Input mesh contains 0 voxels.");
    if (mesh.vertex_per_voxel != 4):
        raise RuntimeError("Input mesh is not a tet mesh.");
    mesh = tet_to_hex(mesh);
    pymesh.save_mesh(args.output_mesh, mesh);

if __name__ == "__main__":
    main();
