#!/usr/bin/env python

"""
Generate icosphere
"""

import argparse
import numpy as np

import pymesh
from pymesh import generate_icosphere
from pymesh import save_mesh

def parse_args():
    parser = argparse.ArgumentParser(description=__doc__);
    parser.add_argument("--radius", "-r",
            type=float, default=1.0, help="sphere radius");
    parser.add_argument("--center", "-c",
            type=float, nargs=3, default=[0, 0, 0],
            help="sphere center");
    parser.add_argument("--refinement", "-R",
            type=int, default=0,
            help="refinement order");
    parser.add_argument("output_mesh", help="output mesh");
    return parser.parse_args();

def main():
    args = parse_args();
    mesh = generate_icosphere(args.radius,
            center = args.center,
            refinement_order = args.refinement);
    save_mesh(args.output_mesh, mesh);

if __name__ == "__main__":
    main();
