#!/usr/bin/env python

"""
Inflate a given wire file into 2D or 3D meshes.
"""

import argparse
import numpy as np
import pymesh.wires

def parse_args():
    parser = argparse.ArgumentParser(description="Inflate wires");
    parser.add_argument("--thickness", "-t", type=float, help="wire thickness");
    parser.add_argument("wire_file", help="input wire file");
    parser.add_argument("mesh_file", help="output mesh file");
    return parser.parse_args();

def main():
    args = parse_args();

    wire_network = pymesh.wires.WireNetwork.create_from_file(args.wire_file);
    inflator = pymesh.wires.Inflator(wire_network);
    inflator.inflate(args.thickness);

    mesh = inflator.mesh;
    pymesh.save_mesh(args.mesh_file, mesh, *mesh.get_attribute_names());

if __name__ == "__main__":
    main();
