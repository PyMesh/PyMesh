#!/usr/bin/env python

"""
Map the volume bounded by a genus 0 mesh to a sphere.
Note that the mapping may not be bijective.
"""

import argparse
import pymesh
import numpy as np
from numpy.linalg import norm
from mean_curvature_flow import mean_curvature_flow
import os.path
import scipy.sparse

def parse_args():
    parser = argparse.ArgumentParser(__doc__);
    parser.add_argument("input_mesh");
    parser.add_argument("output_mesh");
    return parser.parse_args();

def generate_tet_mesh(mesh):
    bbox_min, bbox_max = mesh.bbox;
    l = norm(bbox_max - bbox_min) / 20;

    mesh, t = pymesh.tetrahedralize(mesh,
            l,       # cell size
            2.0,     # max radius/edge ratio
            -1,      # facet distance
            120,     # feature angle
            "tetwild", # engine
            True);   # with timing
    return mesh;

def map_boundary_to_sphere(mesh, basename):
    bd_mesh = pymesh.form_mesh(mesh.vertices, mesh.faces);
    bd_mesh, info = pymesh.remove_isolated_vertices(bd_mesh);
    bd_vertices = np.copy(bd_mesh.vertices);

    assembler = pymesh.Assembler(bd_mesh);
    L = assembler.assemble("graph_laplacian");

    # First, Laplacian smoothing to improve triangle quality.
    for i in range(100):
        c = np.mean(bd_vertices, axis=0);
        bd_vertices = bd_vertices - c;
        r = np.amax(norm(bd_vertices, axis=1));
        bd_vertices /= r;

        bd_vertices += L * bd_vertices;

        if i%10 == 0:
            sphere_mesh = pymesh.form_mesh(bd_vertices, bd_mesh.faces);
            pymesh.save_mesh("{}_flow_{:03}.msh".format(basename, i), sphere_mesh);

    # Then, run mean curvature flow.
    sphere_mesh = pymesh.form_mesh(bd_vertices, bd_mesh.faces);
    sphere_mesh = mean_curvature_flow(sphere_mesh, 100, use_graph_laplacian=True);
    pymesh.save_mesh("{}_flow_final.msh".format(basename), sphere_mesh);
    bd_vertices = sphere_mesh.vertices;

    # Lastly, project vertices onto unit sphere.
    bd_vertex_indices = info["ori_vertex_index"];
    bd_vertex_positions = np.divide(bd_vertices, norm(bd_vertices,
        axis=1).reshape((-1,1)));
    return bd_vertex_indices, bd_vertex_positions;

def init_harmonic_system(L, bd_indices, constraints, is_variable):
    rhs = -L[:,bd_indices].dot(constraints);

    M = L[:, is_variable][is_variable, :];
    rhs = rhs[is_variable, :];
    return M, rhs;

@pymesh.timethis
def solve(M, rhs, solver_type, tol):
    solver = pymesh.SparseSolver.create(solver_type);
    if tol is not None:
        solver.tolerance = tol;
    solver.compute(M);
    x = solver.solve(rhs);
    return x;

def tutte_3D(mesh, bd_vertex_indices, bd_vertex_positions):
    assembler = pymesh.Assembler(mesh);
    L = assembler.assemble("laplacian");

    is_constraint = np.zeros(mesh.num_vertices, dtype=bool);
    is_constraint[bd_vertex_indices] = True;
    is_variable = np.logical_not(is_constraint);

    M, rhs = init_harmonic_system(L, bd_vertex_indices, bd_vertex_positions, is_variable);
    x = solve(M, rhs, "SparseLU", 1e-6);

    out_vertices = np.zeros((mesh.num_vertices, 3));
    out_vertices[is_constraint,:] = bd_vertex_positions;
    out_vertices[is_variable,:] = x;

    return pymesh.form_mesh(out_vertices, np.zeros((0, 3)), mesh.voxels);

def main():
    args = parse_args();
    basename, ext = os.path.splitext(args.output_mesh);

    mesh = pymesh.load_mesh(args.input_mesh);
    mesh = generate_tet_mesh(mesh);
    pymesh.save_mesh("{}_source.msh".format(basename), mesh);

    bd_vertex_indices, bd_vertex_positions = map_boundary_to_sphere(
            mesh, basename);
    out_mesh = tutte_3D(mesh, bd_vertex_indices, bd_vertex_positions);

    tet_orientations = pymesh.get_tet_orientations(out_mesh);
    out_mesh.add_attribute("orientation");
    out_mesh.set_attribute("orientation", tet_orientations);
    pymesh.save_mesh(args.output_mesh, out_mesh, "orientation");

if __name__ == "__main__":
    main();
