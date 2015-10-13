#!/usr/bin/env python

"""
Highlight boundary edges.
"""

import argparse
import numpy as np
import pymesh

def parse_args():
    parser = argparse.ArgumentParser(description=__doc__);
    parser.add_argument("input_mesh", help="input mesh");
    parser.add_argument("output_mesh", help="output mesh");
    return parser.parse_args();

def main():
    args = parse_args();
    mesh = pymesh.load_mesh(args.input_mesh);
    bd_edges = mesh.boundary_edges;
    bd_vertices = np.unique(bd_edges.ravel());
    is_bd_vertices = np.zeros(mesh.num_vertices);
    is_bd_vertices[bd_vertices] = True;
    mesh.add_attribute("boundary_vertices");
    mesh.set_attribute("boundary_vertices", is_bd_vertices);

    pymesh.save_mesh(args.output_mesh, mesh, *mesh.attribute_names);

if __name__ == "__main__":
    main();
