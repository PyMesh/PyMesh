#!/usr/bin/env python

""" Convert a svg file into 2D triangle mesh.
"""

import argparse
import pymesh
import numpy as np
import os.path

def parse_args():
    parser = argparse.ArgumentParser(__doc__);
    parser.add_argument("input_svg");
    parser.add_argument("output_mesh");
    return parser.parse_args();

def main():
    args = parse_args();
    wires = pymesh.wires.WireNetwork();
    wires.load_from_file(args.input_svg);

    vertices, edges, __ = pymesh.remove_duplicated_vertices_raw(wires.vertices, wires.edges, 0.0);
    wires.load(vertices, edges);
    wires.write_to_file(os.path.splitext(args.output_mesh)[0] + ".wire");

    tri = pymesh.triangle();
    tri.points = wires.vertices;
    tri.segments = wires.edges;
    tri.run();

    mesh = tri.mesh;
    regions = tri.regions;
    mesh.add_attribute("regions");
    mesh.set_attribute("regions", regions);
    pymesh.save_mesh(args.output_mesh, mesh, *mesh.attribute_names);

if __name__ == "__main__":
    main();
