#!/usr/bin/env python
import argparse
import numpy as np

from pymesh import mesh_io
from PyWires import WireNetwork, InflatorEngine

def parse_args():
    parser = argparse.ArgumentParser(description="Inflate wires");
    parser.add_argument("--thickness", "-t", type=float, help="wire thickness");
    parser.add_argument("wire_file", help="input wire file");
    parser.add_argument("mesh_file", help="output mesh file");
    return parser.parse_args();

def main():
    args = parse_args();

    wire_network = WireNetwork.create(args.wire_file);
    inflator = InflatorEngine.create("simple", wire_network);
    inflator.set_uniform_thickness(args.thickness);
    inflator.inflate();

    mesh_io.save_mesh_raw(
            args.mesh_file,
            inflator.get_vertices(),
            inflator.get_faces());

if __name__ == "__main__":
    main();
