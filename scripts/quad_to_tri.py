#!/usr/bin/env python

import argparse
import pymesh
import numpy as np

def parse_args():
    parser = argparse.ArgumentParser(description=__doc__);
    parser.add_argument("--keep-symmetry", "-s", action="store_true",
            help="Whether to split quad symmetrically");
    parser.add_argument("input_mesh", help="input quad mesh");
    parser.add_argument("output_mesh", help="output triangle mesh");
    return parser.parse_args();

def main():
    args = parse_args();
    mesh = pymesh.load_mesh(args.input_mesh);
    if (mesh.vertex_per_face != 4):
        raise IOError("input mesh is not a quad mesh");
    mesh = pymesh.quad_to_tri(mesh, args.keep_symmetry);
    pymesh.save_mesh(args.output_mesh, mesh, *mesh.get_attribute_names());

if __name__ == "__main__":
    main();
