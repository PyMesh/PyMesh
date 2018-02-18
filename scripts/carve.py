#!/usr/bin/env python

"""
Carve a given shape from a solid block, one dodecahedron at a time.
"""

import argparse
import pymesh
import math
import numpy as np
import numpy.random
import os.path

def carve_mesh(target_mesh, block, N, batch_size, out_name,
        initial_N=0, save_intermediate=True, debug=False):
    name, ext = os.path.splitext(out_name);
    tree = pymesh.AABBTree();
    tree.load_mesh(target_mesh);

    dodecahedron = pymesh.generate_dodecahedron(1.0, np.zeros(3));
    vertices = dodecahedron.vertices;
    faces = dodecahedron.faces;

    for i in range(initial_N, N, batch_size):
        pts = block.vertices;
        squared_dist, face_indices, closest_pts = \
                tree.look_up_with_closest_points(pts);
        n = np.min([batch_size, N-i, len(pts)]);
        indices = np.argsort(squared_dist)[::-1][:n];
        radii = np.sqrt(squared_dist[indices]);
        centers = pts[indices, :];
        to_remove = [pymesh.form_mesh(
            np.dot(pymesh.Quaternion(numpy.random.rand(4)).to_matrix(), vertices.T * r).T + p, faces)
            for r,p in zip(radii, centers)];
        to_remove = pymesh.merge_meshes(to_remove);
        if debug:
            pymesh.save_mesh("deubg_block.msh", block);
            pymesh.save_mesh("deubg_cut.msh", to_remove);
        block = pymesh.boolean(block, to_remove, "difference", engine="igl");
        block, __ = pymesh.collapse_short_edges(block, 1e-12, False);
        if save_intermediate:
            pymesh.save_mesh("{}_{:06}{}".format(name, i, ext), block);
    return block;

def parse_args():
    parser = argparse.ArgumentParser(description=__doc__);
    parser.add_argument("-N", type=int, default=100,
            help="Number of cuts");
    parser.add_argument("--batch-size", type=int, default=10,
            help="Number of cuts to carry out in a batch");
    parser.add_argument("--save-intermediate", action="store_true",
            help="Save intermediate results");
    parser.add_argument("--initial-block", default=None,
            help="The shape of initial block");
    parser.add_argument("--initial-N", default=0, type=int,
            help="Initial index N");
    parser.add_argument("--debug", action="store_true",
            help="Save debug mesh.");
    parser.add_argument("input_mesh");
    parser.add_argument("output_mesh");
    return parser.parse_args();

def main():
    args = parse_args();
    mesh = pymesh.load_mesh(args.input_mesh);
    if args.initial_block is not None:
        block = pymesh.load_mesh(args.initial_block);
    else:
        bbox_min, bbox_max = mesh.bbox;
        block = pymesh.generate_box_mesh(bbox_min, bbox_max, 2, keep_symmetry=True);
        block = pymesh.form_mesh(block.vertices, block.faces);
        block, __ = pymesh.remove_isolated_vertices(block);

    carved = carve_mesh(mesh, block, args.N,
            args.batch_size,
            args.output_mesh,
            args.initial_N,
            args.save_intermediate,
            args.debug);

    pymesh.save_mesh(args.output_mesh, carved);

if __name__ == "__main__":
    main();
