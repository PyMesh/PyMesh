#!/usr/bin/env python
"""
Triangulate the input.
"""

import argparse
import pymesh
import numpy as np

def parse_args():
    parser = argparse.ArgumentParser(__doc__);
    parser.add_argument("--engine",
            choices=[
                'igl_lexicographic',
                'igl_delaunay', 'shewchuk_triangle',
                'cgal_delaunay', 'geogram_delaunay']);
    parser.add_argument("input_mesh");
    parser.add_argument("output_mesh");
    return parser.parse_args();

def main():
    args = parse_args();
    mesh = pymesh.load_mesh(args.input_mesh, drop_zero_dim=True);
    mesh,__ = pymesh.split_long_edges(mesh, 0.01);
    points = mesh.vertices[mesh.boundary_vertices,:];
    mesh = pymesh.triangulate_beta(points, args.engine);
    pymesh.save_mesh(args.output_mesh, mesh);
    pymesh.timethis.summarize();

if __name__ == "__main__":
    main();
