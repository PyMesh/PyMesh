#!/usr/bin/env python

"""
Add face index attribute to mesh.
"""

import argparse
import pymesh

def parse_args():
    parser = argparse.ArgumentParser(
            description=__doc__);
    parser.add_argument("input_mesh", help="input mesh");
    parser.add_argument("output_mesh", help="output mesh");
    return parser.parse_args();

def main():
    args = parse_args();
    mesh = pymesh.load_mesh(args.input_mesh);
    mesh.add_attribute("vertex_index");
    mesh.add_attribute("face_index");
    mesh.add_attribute("voxel_index");
    pymesh.save_mesh(args.output_mesh, mesh, *mesh.attribute_names);

if __name__ == "__main__":
    main();
