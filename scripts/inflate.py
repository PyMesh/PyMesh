#!/usr/bin/env python

"""
Inflate a given wire file into 2D or 3D meshes.
"""

import argparse
import numpy as np
import pymesh.wires

def parse_args():
    parser = argparse.ArgumentParser(description=__doc__);
    parser.add_argument("--thickness", "-t", type=float, help="wire thickness",
            required=True);
    parser.add_argument("--refinement-order", "-r", type=int, default=1,
            help="refinement order");
    parser.add_argument("--refinement-method", "-m", choices=("simple", "loop"),
            default="simple", help="refinement method");
    parser.add_argument("wire_file", help="input wire file");
    parser.add_argument("mesh_file", help="output mesh file");
    return parser.parse_args();

def main():
    args = parse_args();

    wire_network = pymesh.wires.WireNetwork.create_from_file(args.wire_file);
    inflator = pymesh.wires.Inflator(wire_network);
    inflator.set_refinement(
            order = args.refinement_order,
            method = args.refinement_method);
    inflator.inflate(args.thickness);

    mesh = inflator.mesh;
    pymesh.save_mesh(args.mesh_file, mesh, *mesh.get_attribute_names());

if __name__ == "__main__":
    main();
