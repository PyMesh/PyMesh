#!/usr/bin/env python

"""
Extract the surface triangle mesh from a tet mesh.
"""

import argparse
import numpy as np
import pymesh

def parse_args():
    parser = argparse.ArgumentParser(__doc__);
    parser.add_argument("input_mesh");
    parser.add_argument("output_mesh");
    return parser.parse_args();

def main():
    args = parse_args();
    mesh = pymesh.load_mesh(args.input_mesh);
    mesh = pymesh.form_mesh(mesh.vertices, mesh.faces);
    mesh, __ = pymesh.remove_isolated_vertices(mesh);
    pymesh.save_mesh(args.output_mesh, mesh);

if __name__ == "__main__":
    main();
