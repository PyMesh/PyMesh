#!python

"""
Merge multiple meshes into a single mesh.
"""

import argparse
import logging
import numpy as np
import pymesh
from pymesh import load_mesh, save_mesh
from pymesh import merge_meshes

def parse_args():
    parser = argparse.ArgumentParser(
            description=__doc__);
    parser.add_argument("-o", "--output", required=True, help="output mesh");
    parser.add_argument("input_meshes", nargs="+", help="input meshes");
    return parser.parse_args();

def main():
    args = parse_args();

    input_meshes = [load_mesh(filename) for filename in args.input_meshes];
    output_mesh = merge_meshes(input_meshes);
    save_mesh(args.output, output_mesh,
            *output_mesh.get_attribute_names());

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO);
    main();
