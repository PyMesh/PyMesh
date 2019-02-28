#!/usr/bin/env python

import argparse
import pymesh
import numpy as np
from numpy.linalg import norm
import scipy.sparse

def parse_args():
    parser = argparse.ArgumentParser(description=__doc__);
    parser.add_argument("--time-step", type=float, default=0.001,
            help="Integration time step");
    parser.add_argument("--num-itrs", type=int, default=1,
            help="Number of iterations");
    parser.add_argument("--use-graph-laplacian", action="store_true",
            help="Whether to use graph Laplacian instead of cotan Laplacian")
    parser.add_argument("input_mesh");
    parser.add_argument("output_mesh");
    return parser.parse_args();

def normalize_mesh(vertices, faces):
    centroid = np.mean(vertices, axis=0);
    vertices -= centroid;
    radii = norm(vertices, axis=1);
    vertices /= np.amax(radii);
    return pymesh.form_mesh(vertices, faces);

def integrate(mesh, time_step, use_graph_laplacian):
    assembler = pymesh.Assembler(mesh);
    if use_graph_laplacian:
        L = -assembler.assemble("graph_laplacian");
    else:
        L = assembler.assemble("laplacian");
    M = assembler.assemble("mass");

    bbox_min, bbox_max = mesh.bbox;
    s = np.amax(bbox_max - bbox_min);

    S = M + (time_step * s) * L;
    solver = pymesh.SparseSolver.create("SparseLU");
    solver.compute(S);
    vertices = solver.solve(M * mesh.vertices);

    return normalize_mesh(vertices, mesh.faces);

def mean_curvature_flow(mesh, num_itrs=1, time_step=1e-3,
        use_graph_laplacian=False):
    mesh = normalize_mesh(np.copy(mesh.vertices), mesh.faces);
    for i in range(num_itrs):
        try:
            mesh = integrate(mesh, time_step, use_graph_laplacian);
        except Exception as e:
            print(e);
            print("Mean curvature flow failed at iteration {}".format(i));
            time_step /= 2;
            if time_step < 1e-6:
                return mesh;
            continue;
    return mesh;

def main():
    args = parse_args();
    mesh = pymesh.load_mesh(args.input_mesh);
    mesh = mean_curvature_flow(mesh, args.num_itrs, args.time_step,
            args.use_graph_laplacian);
    pymesh.save_mesh(args.output_mesh, mesh);

if __name__ == "__main__":
    main();
