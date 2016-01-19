#!/usr/bin/env python

""" Slice a given 3D model
"""

import argparse
import pymesh
import numpy as np
import os.path

def parse_args():
    parser = argparse.ArgumentParser(description=__doc__);
    parser.add_argument("-N", type=int, help="number of slices");
    parser.add_argument("input_mesh");
    parser.add_argument("output_slices");
    return parser.parse_args();

def slice_mesh(mesh, N):
    bbox_min, bbox_max = mesh.bbox;
    min_corner = [bbox_min[0] -1.0, bbox_min[1] - 1.0, bbox_min[2] - 1.0];
    output_wires = [];
    for i in range(N):
        ratio = float(i) / float(N);
        slice_val = bbox_min[0] * ratio + bbox_max[0] * (1-ratio);
        max_corner = [slice_val, bbox_max[1] + 1.0, bbox_max[2] + 1.0];
        box = pymesh.generate_box_mesh(min_corner, max_corner);
        diff = pymesh.boolean(box, mesh, "difference");
        #pymesh.save_mesh("tmp_{}.msh".format(i), diff);
        vertices = diff.vertices;
        y_out_range = np.logical_or(
                    vertices[:,1] < bbox_min[1],
                    vertices[:,1] > bbox_max[1]);
        z_out_range = np.logical_or(
                    vertices[:,2] < bbox_min[2],
                    vertices[:,2] > bbox_max[2]);
        on_outside = np.logical_or(y_out_range, z_out_range);

        if not np.any(on_outside): continue;

        edges = [];
        for f in diff.faces:
            if np.sum(on_outside[f]) == 1:
                if on_outside[f[0]]:
                    edges.append([f[1], f[2]]);
                if on_outside[f[1]]:
                    edges.append([f[2], f[0]]);
                if on_outside[f[2]]:
                    edges.append([f[0], f[1]]);

        if len(edges) == 0: continue;
        edges = np.array(edges, dtype=int);

        below_ground = vertices[:,1] < 0.5*(bbox_min[1]+bbox_max[1]);
        edges_below_ground = np.any(below_ground[edges], axis=1);
        edges = edges[np.logical_not(edges_below_ground)];

        vertices, edges, __ = \
                pymesh.remove_isolated_vertices_raw(vertices, edges);
        wires = pymesh.wires.WireNetwork.create_from_data(vertices, edges);
        output_wires.append(wires);
        print('.',end="",flush=True)
    print("done");

    return output_wires;

def main():
    args = parse_args();
    mesh = pymesh.load_mesh(args.input_mesh);
    slices = slice_mesh(mesh, args.N);

    v_count = 0;
    vertices = [];
    edges = [];
    basename, ext = os.path.splitext(args.output_slices);
    for i,s in enumerate(slices):
        slice_file = "{}_{:06}{}".format(basename, i, ext);
        s.write_to_file(slice_file);
        vertices.append(s.vertices);
        edges.append(s.edges + v_count);
        v_count += s.num_vertices;

    all_wires = pymesh.wires.WireNetwork.create_from_data(
            np.vstack(vertices), np.vstack(edges));
    all_wires.write_to_file(args.output_slices);

if __name__ == "__main__":
    main();

