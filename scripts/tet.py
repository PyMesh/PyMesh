#!/usr/bin/env python
import argparse
import numpy as np
import os
import os.path
import pymesh
import sys

def parse_args():
    parser = argparse.ArgumentParser(description="Tet mesh generation");
    parser.add_argument("--engine", help="tet mesh engine",
            choices = ("cgal", "tetgen", "geogram", "quartet"),
            default="tetgen");
    parser.add_argument("--face-size",
            help="max circumradius of surface triangles",
            type=float, default=-1.0);
    parser.add_argument("--cell-size", help="max circumradius of tets",
            type=float, default=-1.0);
    parser.add_argument("in_mesh", help="input mesh");
    parser.add_argument("out_mesh", help="output mesh");
    args = parser.parse_args();
    return args;

def main():
    args = parse_args();
    mesh = pymesh.load_mesh(args.in_mesh);

    tet_mesh = pymesh.tetrahedralize(mesh,
            args.face_size,
            args.cell_size,
            args.engine);
    pymesh.save_mesh(args.out_mesh, tet_mesh);

if __name__ == "__main__":
    main();

