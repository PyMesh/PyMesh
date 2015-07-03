#!/usr/bin/env python

"""
Compute curvatures of the input mesh.
"""

import pymesh
import argparse

def parse_args():
    parser = argparse.ArgumentParser(description=__doc__);
    parser.add_argument("input_mesh", help="input mesh");
    parser.add_argument("output_mesh", help="output mesh");
    return parser.parse_args();

def main():
    args = parse_args();

    mesh = pymesh.load_mesh(args.input_mesh);
    mesh.add_attribute("vertex_gaussian_curvature");
    mesh.add_attribute("vertex_mean_curvature");
    pymesh.save_mesh(args.output_mesh, mesh, *mesh.get_attribute_names());

if __name__ == "__main__":
    main();

