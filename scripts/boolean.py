#!/usr/bin/env python

import argparse
import numpy as np
import pymesh

def parse_args():
    parser = argparse.ArgumentParser(description="Boolean operation");
    parser.add_argument("--exact", default=None, help="Exact output file");
    parser.add_argument("--timing", action="store_true", help="Report timing");
    parser.add_argument("--engine",
            choices=("cork", "clipper", "igl", "cgal", "corefinement", "carve",
                "quick_csg", "bsp"),
            default="igl");
    parser.add_argument("operation", default="intersection",
            choices=("intersection", "union", "difference",
                "symmetric_difference"));
    parser.add_argument("input_mesh_1", help="first input mesh");
    parser.add_argument("input_mesh_2", help="second input mesh");
    parser.add_argument("output_mesh", help="output mesh");
    args = parser.parse_args();
    return args;

def main():
    args = parse_args();
    mesh1 = pymesh.load_mesh(args.input_mesh_1);
    mesh2 = pymesh.load_mesh(args.input_mesh_2);
    if args.timing:
        mesh, t = pymesh.boolean(mesh1, mesh2, args.operation, args.engine,
                with_timing = args.timing,
                exact_mesh_file = args.exact);
        print("Timing: {}".format(t));
    else:
        mesh = pymesh.boolean(mesh1, mesh2, args.operation, args.engine,
                exact_mesh_file = args.exact);
    pymesh.save_mesh(args.output_mesh, mesh, *mesh.get_attribute_names());

if __name__ == "__main__":
    main();
