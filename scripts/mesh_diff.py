#!/usr/bin/env python

"""
Compare two meshes to see if they represent the same volume.
Warning: this is a Monte-Carlo algorithm.
"""

import argparse
import pymesh
import numpy as np

from meshstat import load_info, dump_info

def grid_sample(bbox_min, bbox_max, num_samples):
    x_samples = np.linspace(bbox_min[0], bbox_max[0], num_samples);
    y_samples = np.linspace(bbox_min[1], bbox_max[1], num_samples);
    z_samples = np.linspace(bbox_min[2], bbox_max[2], num_samples);

    X, Y, Z = np.meshgrid(x_samples, y_samples, z_samples);

    return np.array([X.ravel(), Y.ravel(), Z.ravel()], order="C").T;

def random_sample(bbox_min, bbox_max, num_samples):
    samples = np.random.rand(num_samples, 3);
    samples[:,0] = samples[:,0] * bbox_min[0] + (1.0 - samples[:,0]) * bbox_max[0]
    samples[:,1] = samples[:,1] * bbox_min[1] + (1.0 - samples[:,1]) * bbox_max[1]
    samples[:,2] = samples[:,2] * bbox_min[2] + (1.0 - samples[:,2]) * bbox_max[2]
    return samples;

def parse_args():
    parser = argparse.ArgumentParser(description=__doc__);
    parser.add_argument("--num-samples", "-n", type=int, default=5,
            help="number of samples");
    parser.add_argument("--tol", type=float, default=1e-6,
            help="winding number tolerance");
    parser.add_argument("--output", "-o", help="Highlighted output",
            default=None);
    parser.add_argument("--export", "-e", help="export information",
            action="store_true");
    parser.add_argument("input_mesh_1", help="first input mesh");
    parser.add_argument("input_mesh_2", help="secondinput mesh");
    return parser.parse_args();

def main():
    args = parse_args();
    mesh_1 = pymesh.load_mesh(args.input_mesh_1);
    mesh_2 = pymesh.load_mesh(args.input_mesh_2);
    assert(mesh_1.dim == 3);
    assert(mesh_2.dim == 3);

    bbox_min_1, bbox_max_1 = mesh_1.bbox;
    bbox_min_2, bbox_max_2 = mesh_2.bbox;

    bbox_min = np.minimum(bbox_min_1, bbox_min_2);
    bbox_max = np.maximum(bbox_max_1, bbox_max_2);

    #queries = grid_sample(bbox_min, bbox_max, args.num_samples);
    queries = random_sample(bbox_min, bbox_max, args.num_samples);

    winding_number_1 = pymesh.compute_winding_number(mesh_1, queries) > 0.5;
    winding_number_2 = pymesh.compute_winding_number(mesh_2, queries) > 0.5;

    diff = np.logical_xor(winding_number_1, winding_number_2);
    num_diff = np.count_nonzero(diff);
    print("Winding numbers of {} out of {} samples differ".format(
        num_diff, len(queries)));

    if args.output is not None:
        r = np.amax(bbox_max - bbox_min) * 0.01;
        box = pymesh.generate_box_mesh(np.ones(3) * -r, np.ones(3) * r);

        vertices = [];
        faces = [];
        for i in range(len(queries)):
            vertices.append(box.vertices + queries[i]);
            faces.append(box.faces + box.num_vertices * i);
        vertices = np.vstack(vertices);
        faces = np.vstack(faces);
        mesh = pymesh.form_mesh(vertices, faces);
        mesh.add_attribute("diff");
        mesh.set_attribute("diff", np.repeat(diff, box.num_faces));
        pymesh.save_mesh(args.output, mesh, "diff");

    if args.export:
        info = load_info(args.input_mesh_2);
        info["diff"] = num_diff
        dump_info(args.input_mesh_2, info);

if __name__ == "__main__":
    main();
