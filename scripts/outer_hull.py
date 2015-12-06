#!/usr/bin/env python

"""
Compute the outer hull of the input mesh.
"""

import argparse
import pymesh
import os.path

def parse_args():
    parser = argparse.ArgumentParser(description=__doc__);
    parser.add_argument("--engine", choices=["auto", "igl"], default="auto");
    parser.add_argument("--recursive", "-R",
            help="Recursively peel all outer hull layers",
            default=None, action="store_true");
    parser.add_argument("input_mesh", help="input mesh");
    parser.add_argument("output_mesh", help="output mesh");
    return parser.parse_args();

def main():
    args = parse_args();
    mesh = pymesh.load_mesh(args.input_mesh);
    #mesh, __ = pymesh.remove_degenerated_triangles(mesh, 1000);
    #pymesh.save_mesh("cleaned.msh", mesh);

    result = pymesh.compute_outer_hull(mesh, engine=args.engine,
            all_layers=args.recursive);

    if args.recursive:
        basename, ext = os.path.splitext(args.output_mesh);
        for i,outer_hull in enumerate(result):
            out_name = "{}_{}{}".format(basename, i, ext);
            pymesh.save_mesh(out_name, outer_hull,
                    *outer_hull.get_attribute_names());
    else:
        pymesh.save_mesh(args.output_mesh, result,
                *result.get_attribute_names());

if __name__ == "__main__":
    main();
