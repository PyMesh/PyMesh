#!/usr/bin/env python

""" Convert a svg file into 2D triangle mesh.
"""

import argparse
import pymesh
import numpy as np
import os.path

def parse_args():
    parser = argparse.ArgumentParser(__doc__);
    parser.add_argument("--engine", help="Triangulation engine", choices=(
                "triangle_conforming_delaunay",
                "triangle_constrained_delaunay",
                "cgal_constrained_delaunay",
                "cgal_conforming_delaunay",
                "jigsaw_frontal_delaunay"),
            default="triangle_conforming_delaunay");
    parser.add_argument("input_svg");
    parser.add_argument("output_mesh");
    return parser.parse_args();

def cleanup(wires):
    vertices, edges, __ = pymesh.remove_duplicated_vertices_raw(wires.vertices, wires.edges, 0.0);

    # Trim zero dimension.
    if wires.dim == 3:
        assert(np.all(vertices[:,2] == 0));
        vertices = vertices[:, [0,1]];

    # Remove duplicated edges.
    ordered_edges = np.sort(edges, axis=1);
    __, unique_edge_ids = np.unique(ordered_edges, return_index=True,
            axis=0);
    edges = edges[unique_edge_ids, :];

    # Remove topologically degenerate edges.
    wires.load(vertices, edges);
    is_not_topologically_degenerate = edges[:,0] != edges[:,1];
    if not np.all(is_not_topologically_degenerate):
        wires.filter_edges(is_not_topologically_degenerate);

    return wires;

def main():
    args = parse_args();
    wires = pymesh.wires.WireNetwork();
    wires.load_from_file(args.input_svg);
    wires = cleanup(wires);
    wires.write_to_file(os.path.splitext(args.output_mesh)[0] + ".wire");

    mesh = pymesh.triangulate_beta(wires.vertices, wires.edges,
            engine=args.engine);

    pymesh.save_mesh(args.output_mesh, mesh);

if __name__ == "__main__":
    main();
