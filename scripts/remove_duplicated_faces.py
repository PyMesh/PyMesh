#!/usr/bin/env python

"""
Remove duplicated faces.
"""

import argparse
import pymesh

def parse_args():
    parser = argparse.ArgumentParser(description=__doc__);
    parser.add_argument("input_mesh", help="input mesh");
    parser.add_argument("output_mesh", help="output mesh");
    return parser.parse_args();

def main():
    args = parse_args();
    mesh = pymesh.load_mesh(args.input_mesh);
    mesh, __ = pymesh.remove_duplicated_faces(mesh);
    pymesh.save_mesh(args.output_mesh, mesh);

if __name__ == "__main__":
    main();
