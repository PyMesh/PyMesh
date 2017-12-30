#!/usr/bin/env python

""" This script checks the mesh quality by solving a simple harmonic PDE over
the mesh.
"""

import argparse
import logging
import numpy as np
import numpy.linalg
import pymesh

def parse_args():
    parser = argparse.ArgumentParser(description=__doc__);
    parser.add_argument("input_mesh");
    parser.add_argument("output_mesh");
    return parser.parse_args();

def evaluate_one_over_r(mesh):
    eps = 0.1;
    bbox_min, bbox_max = mesh.bbox;
    charge_loc = bbox_max + np.ones(mesh.dim) * eps;

    r = numpy.linalg.norm(mesh.vertices - charge_loc, axis=1);
    return 1.0 / r;

def main():
    args = parse_args();
    mesh = pymesh.load_mesh(args.input_mesh);
    assert(mesh.dim == 3);
    assert(mesh.vertex_per_voxel == 4);
    assert(mesh.num_voxels > 0);

    target_solution = evaluate_one_over_r(mesh);
    bd_indices = np.unique(mesh.faces.ravel());
    bd_values = target_solution[bd_indices];

    solver = pymesh.HarmonicSolver.create(mesh);
    solver.order = 1;
    solver.boundary_indices = bd_indices;
    solver.boundary_values = bd_values;
    solver.pre_process();
    solver.solve();

    sol = solver.solution.ravel();

    err = numpy.linalg.norm(sol - target_solution);
    print("err: {}".format(err));

    mesh.add_attribute("target_solution");
    mesh.set_attribute("target_solution", target_solution);
    mesh.add_attribute("solution");
    mesh.set_attribute("solution", sol);
    mesh.add_attribute("error");
    mesh.set_attribute("error", target_solution - sol);
    pymesh.save_mesh(args.output_mesh, mesh, *mesh.attribute_names);

if __name__ == "__main__":
    main();
