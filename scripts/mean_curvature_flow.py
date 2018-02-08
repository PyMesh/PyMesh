#!/usr/bin/env python

import argparse
import pymesh
import numpy as np

def parse_args():
    parser = argparse.ArgumentParser(description=__doc__);
    parser.add_argument("--time-step", type=float, default=0.001,
            help="Integration time step");
    parser.add_argument("--num-itrs", type=int, default=1,
            help="Number of iterations");
    parser.add_argument("input_mesh");
    parser.add_argument("output_mesh");
    return parser.parse_args();

def normalize_mesh(vertices, faces):
    centroid = np.mean(vertices, axis=0);
    vertices -= centroid;
    return pymesh.form_mesh(vertices, faces);

def integrate(mesh, time_step):
    assembler = pymesh.Assembler(mesh);
    L = assembler.assemble("laplacian");
    M = assembler.assemble("mass");

    bbox_min, bbox_max = mesh.bbox;
    s = np.amax(bbox_max - bbox_min);

    S = M + (time_step * s) * L;
    solver = pymesh.SparseSolver.create("LDLT");
    solver.compute(S);
    vertices = solver.solve(M * mesh.vertices);

    return normalize_mesh(vertices, mesh.faces);

def main():
    args = parse_args();
    mesh = pymesh.load_mesh(args.input_mesh);

    for i in range(args.num_itrs):
        mesh = integrate(mesh, args.time_step);

    pymesh.save_mesh(args.output_mesh, mesh);

if __name__ == "__main__":
    main();
