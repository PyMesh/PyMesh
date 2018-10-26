#!/usr/bin/env python
"""
Refine a given mesh
"""

import argparse
import pymesh
import numpy as np

def parse_args():
    parser = argparse.ArgumentParser(__doc__);
    parser.add_argument("--max-edge-length", "-l", type=float,
            help="maximum edge length", default=1.0);
    parser.add_argument("input_mesh");
    parser.add_argument("output_mesh");
    return parser.parse_args();

def main():
    args = parse_args();
    mesh = pymesh.load_mesh(args.input_mesh);
    out_mesh, info = pymesh.split_long_edges(mesh, args.max_edge_length);

    if mesh.has_attribute("corner_texture"):
        pymesh.map_corner_attribute(mesh, out_mesh, "corner_texture");

    pymesh.save_mesh(args.output_mesh, out_mesh, *out_mesh.attribute_names);

if __name__ == "__main__":
    main();
