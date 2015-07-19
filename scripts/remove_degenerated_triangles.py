#!/usr/bin/env python

import argparse
import pymesh

def parse_args():
    parser = argparse.ArgumentParser(
            description="Remove degenerated triangles");
    parser.add_argument("--num-iterations", "-n", type=int,
            default=5, help="max number of iterations");
    parser.add_argument("input_mesh", help="input triangular mesh");
    parser.add_argument("output_mesh", help="output triangular mesh");
    return parser.parse_args();

def main():
    args = parse_args();
    mesh = pymesh.load_mesh(args.input_mesh);
    mesh = pymesh.remove_degenerated_triangles(mesh, args.num_iterations);
    pymesh.save_mesh(args.output_mesh, mesh);

if __name__ == "__main__":
    main();
