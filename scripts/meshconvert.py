#!/usr/bin/env python

"""
Convert input mesh into a different format.
"""

import argparse
import pymesh

def parse_args():
    parser = argparse.ArgumentParser(description=__doc__);
    parser.add_argument("--ascii", "-A", action="store_true",
            help="use ASCII format if available");
    parser.add_argument("input_mesh", help="input mesh");
    parser.add_argument("output_mesh", help="output mesh");
    return parser.parse_args();

def main():
    args = parse_args();
    mesh = pymesh.load_mesh(args.input_mesh);
    pymesh.save_mesh(args.output_mesh, mesh,
            ascii=args.ascii, *mesh.attribute_names);

if __name__ == "__main__":
    main();

