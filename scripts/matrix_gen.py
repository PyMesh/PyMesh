#!/usr/bin/env python

""" Assemble the specified FEM matrix.
"""

import argparse
import pymesh
import scipy.sparse

def parse_args():
    parser = argparse.ArgumentParser(__doc__);
    parser.add_argument("--type", "-t", help="The type of matrix",
            choices=["stiffness", "mass", "lumped_mass", "laplacian",
                "displacement_strain", "elasticity_tensor",
                "engineer_strain_stress", "rigid_motion", "gradient"]);
    parser.add_argument("input_mesh");
    parser.add_argument("output_matrix");
    return parser.parse_args();

def main():
    args = parse_args();
    mesh = pymesh.load_mesh(args.input_mesh);
    assembler = pymesh.Assembler(mesh);
    M = assembler.assemble(args.type);
    scipy.sparse.save_npz(args.output_matrix, M);

if __name__ == "__main__":
    main();
