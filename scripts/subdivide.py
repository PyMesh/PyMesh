#!/usr/bin/env python

"""
Subdivide input mesh using the specified method and order.
"""

import argparse
import numpy as np
import pymesh

def parse_args():
    parser = argparse.ArgumentParser(description=__doc__);
    parser.add_argument("--method", default="simple", choices=("simple", "loop"));
    parser.add_argument("--order", default=1, type=int);
    parser.add_argument("input_mesh", help="input mesh");
    parser.add_argument("output_mesh", help="output mesh");
    args = parser.parse_args();
    return args;

def main():
    args = parse_args();
    mesh = pymesh.load_mesh(args.input_mesh);
    mesh = pymesh.subdivide(mesh, args.order, args.method);
    pymesh.save_mesh(args.output_mesh, mesh);

if __name__ == "__main__":
    main();
