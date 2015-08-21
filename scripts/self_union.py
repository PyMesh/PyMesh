#!/usr/bin/env python

import argparse
import numpy as np
import pymesh

def parse_args():
    parser = argparse.ArgumentParser(description="Self union");
    parser.add_argument("input_mesh", help="Input mesh");
    parser.add_argument("output_mesh", help="Output mesh");
    return parser.parse_args();

def main():
    args = parse_args();
    mesh = pymesh.load_mesh(args.input_mesh);
    empty_mesh = pymesh.form_mesh(
            np.zeros((0, mesh.dim)),
            np.zeros((0, mesh.vertex_per_face)));
    result = pymesh.boolean(mesh, empty_mesh, "union", engine="igl");
    pymesh.save_mesh(args.output_mesh, result);

if __name__ == "__main__":
    main();
