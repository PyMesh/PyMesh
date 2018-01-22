#!/usr/bin/env python

""" This script checks the mesh quality by solving a simple harmonic PDE over
the mesh.
"""

import argparse
import logging
import numpy as np
import numpy.linalg
import pymesh
import math

def parse_args():
    parser = argparse.ArgumentParser(description=__doc__);
    parser.add_argument("--charge-distribution", "-c", choices=["sphere", "center"],
            default="sphere");
    parser.add_argument("--output-kernels", action="store_true");
    parser.add_argument("input_mesh");
    parser.add_argument("output_mesh");
    return parser.parse_args();

def test_function(charges):
    """
    f(x,y,z) = 1.0 / sqrt(x*x + y*y + z*z);
    """
    return lambda pts: np.sum([1.0 / numpy.linalg.norm(pts - c, axis=1)
        for c in charges], axis=0);

def test_function_grad(charges):
    """
                  [ -1.0*x/(x**2 + y**2 + z**2)**(3/2) ]
    grad(x,y,z) = [ -1.0*y/(x**2 + y**2 + z**2)**(3/2) ]
                  [ -1.0*z/(x**2 + y**2 + z**2)**(3/2) ]
    """
    return lambda pts: np.sum([-(pts-c) / np.power(numpy.linalg.norm(pts-c, axis=1), 3)[:,np.newaxis]
        for c in charges], axis=0);

def get_quadrature_pts(mesh):
    """ Generate second degree Gaussian quadrature points for tets.
    """
    vertices = mesh.vertices;
    tets = mesh.voxels;

    b = 0.1381966011250105;
    a = 1.0 - 3 * b;

    q_pts = np.ndarray((mesh.num_voxels * 4, 3));
    q_pts[0:mesh.num_voxels*4:4] = \
            a*vertices[tets[:,0]] + \
            b*vertices[tets[:,1]] + \
            b*vertices[tets[:,2]] + \
            b*vertices[tets[:,3]];
    q_pts[1:mesh.num_voxels*4:4] = \
            b*vertices[tets[:,0]] + \
            a*vertices[tets[:,1]] + \
            b*vertices[tets[:,2]] + \
            b*vertices[tets[:,3]];
    q_pts[2:mesh.num_voxels*4:4] = \
            b*vertices[tets[:,0]] + \
            b*vertices[tets[:,1]] + \
            a*vertices[tets[:,2]] + \
            b*vertices[tets[:,3]];
    q_pts[3:mesh.num_voxels*4:4] = \
            b*vertices[tets[:,0]] + \
            b*vertices[tets[:,1]] + \
            b*vertices[tets[:,2]] + \
            a*vertices[tets[:,3]];
    return q_pts;

def interpolate_at_quadrature_pts(mesh, sol_values, sol_grads):
    tets = mesh.voxels;
    b = 0.1381966011250105;
    a = 1.0 - 3 * b;

    values = np.ndarray(mesh.num_voxels * 4);
    values[0:mesh.num_voxels*4:4] = \
            a*sol_values[tets[:,0]] + \
            b*sol_values[tets[:,1]] + \
            b*sol_values[tets[:,2]] + \
            b*sol_values[tets[:,3]];
    values[1:mesh.num_voxels*4:4] = \
            b*sol_values[tets[:,0]] + \
            a*sol_values[tets[:,1]] + \
            b*sol_values[tets[:,2]] + \
            b*sol_values[tets[:,3]];
    values[2:mesh.num_voxels*4:4] = \
            b*sol_values[tets[:,0]] + \
            b*sol_values[tets[:,1]] + \
            a*sol_values[tets[:,2]] + \
            b*sol_values[tets[:,3]];
    values[3:mesh.num_voxels*4:4] = \
            b*sol_values[tets[:,0]] + \
            b*sol_values[tets[:,1]] + \
            b*sol_values[tets[:,2]] + \
            a*sol_values[tets[:,3]];
    # Grad is elementwise const.
    grads = np.repeat(sol_grads, 4, axis=0);

    return values, grads;

def interpolate_at_centroids(mesh, sol_values, sol_grads):
    tets = mesh.voxels;
    values = np.zeros(mesh.num_voxels);
    values += sol_values[tets[:,0]];
    values += sol_values[tets[:,1]];
    values += sol_values[tets[:,2]];
    values += sol_values[tets[:,3]];
    values /= 4.0;
    return values, sol_grads;

def fit_into_unit_sphere(mesh):
    bbox_min, bbox_max = mesh.bbox;
    bbox_radius = numpy.linalg.norm(bbox_max - bbox_min) * 0.5;
    bbox_center = 0.5 * (bbox_min + bbox_max);
    if bbox_radius == 0.0:
        raise IOError("Input mesh is degenerate to a single point.");

    vertices = mesh.vertices;
    tris = mesh.faces;
    tets = mesh.voxels;
    vertices = (vertices - bbox_center) / bbox_radius;
    return pymesh.form_mesh(vertices, tris, tets);

def main():
    args = parse_args();
    mesh = pymesh.load_mesh(args.input_mesh);
    num_tets = mesh.num_voxels;
    assert(mesh.dim == 3);
    assert(mesh.vertex_per_voxel == 4);
    assert(num_tets > 0);
    mesh = fit_into_unit_sphere(mesh);
    mesh.add_attribute("voxel_centroid");
    assembler = pymesh.Assembler(mesh);

    if args.charge_distribution == "sphere":
        ball = pymesh.generate_icosphere(1.25 + 1e-3, np.zeros(3), 0);
        charges = ball.vertices;
    elif args.charge_distribution == "center":
        charges = np.zeros(3);
    else:
        raise NotImplementedError("Unsupported charge distribution ({})."\
                .format(args.charge_distribution));

    if args.output_kernels:
        kernels = pymesh.merge_meshes([
            pymesh.generate_icosphere(0.05, v, 2) for v in charges ]);
        pymesh.save_mesh("kernel.msh", kernels);
    f = test_function(charges);
    g = test_function_grad(charges);

    v_pts = mesh.vertices;
    q_pts = get_quadrature_pts(mesh);
    c_pts = mesh.get_voxel_attribute("voxel_centroid");

    v_values = f(v_pts);
    q_values = f(q_pts);
    c_values = f(c_pts);
    q_grad = g(q_pts);
    c_grad = g(c_pts);
    c_grad_norm = numpy.linalg.norm(c_grad, axis=1);
    q_grad_norm = numpy.linalg.norm(q_grad, axis=1);
    #for v,val in zip(v_pts, v_values):
    #    print(v, val);

    assert(len(v_values) == len(v_pts));
    assert(len(q_values) == len(q_pts));
    assert(len(c_values) == len(c_pts));
    assert(len(q_grad) == len(q_pts));
    assert(len(c_grad) == len(c_pts));

    bd_indices = np.unique(mesh.faces.ravel()).astype(int);
    bd_values = v_values[bd_indices];

    solver = pymesh.HarmonicSolver.create(mesh);
    solver.order = 1;
    solver.boundary_indices = bd_indices;
    solver.boundary_values = bd_values;
    solver.pre_process();
    solver.solve();

    sol_values = solver.solution.ravel();
    G = assembler.assemble("gradient");
    sol_grad = (G * sol_values).reshape((-1,3), order="C");
    sol_q_values, sol_q_grad = \
            interpolate_at_quadrature_pts(mesh, sol_values, sol_grad);
    sol_c_values, sol_c_grad = \
            interpolate_at_centroids(mesh, sol_values, sol_grad);

    v_err = sol_values - v_values;
    c_err = sol_c_values - c_values;
    q_err = sol_q_values - q_values;
    v_rel_err = np.divide(v_err, v_values);
    c_rel_err = np.divide(c_err, c_values);
    q_rel_err = np.divide(q_err, q_values);

    c_grad_err = sol_c_grad - c_grad;
    q_grad_err = sol_q_grad - q_grad;
    c_grad_err_norm = numpy.linalg.norm(c_grad_err, axis=1);
    q_grad_err_norm = numpy.linalg.norm(q_grad_err, axis=1);
    c_grad_rel_err_norm = np.divide(c_grad_err_norm, c_grad_norm);
    q_grad_rel_err_norm = np.divide(q_grad_err_norm, q_grad_norm);

    print("num_tets: {}".format(num_tets));

    print("==Ground Truth==");
    print("max solution at nodes: {}".format(np.amax(np.absolute(v_values))));
    print("max solution at centroids: {}".format(np.amax(np.absolute(c_values))));
    print("max solution at quadrature pts: {}".format(np.amax(np.absolute(q_values))));
    print("max grad solution at centroids: {}".format(np.amax(c_grad_norm)));
    print("max grad solution at quadrature pts: {}".format(np.amax(q_grad_norm)));

    print("L2 solution at nodes: {}".format(numpy.linalg.norm(v_values)));
    print("L2 solution at centroids: {}".format(numpy.linalg.norm(c_values)));
    print("L2 solution at quadrature pts: {}".format(numpy.linalg.norm(q_values)));
    print("L2 grad solution at centroids: {}".format(numpy.linalg.norm(c_grad_norm)));
    print("L2 grad solution at quadrature pts: {}".format(numpy.linalg.norm(q_grad_norm)));

    print("sqrt ave at nodes: {}".format(math.sqrt(np.mean(np.square(v_values)))));
    print("sqrt ave at centroids: {}".format(math.sqrt(np.mean(np.square(c_values)))));
    print("sqrt ave at quadrature pts: {}".format(math.sqrt(np.mean(np.square(q_values)))));
    print("sqrt ave grad solution at centroids: {}".format(math.sqrt(np.mean(np.square(c_grad_norm)))));
    print("sqrt ave grad solution at quadrature pts: {}".format(math.sqrt(np.mean(np.square(q_grad_norm)))));

    print("==Absolute errors==");
    print("max error at nodes: {}".format(np.amax(np.absolute(v_err))));
    print("max error at centroids: {}".format(np.amax(np.absolute(c_err))));
    print("max error at quadrature pts: {}".format(np.amax(np.absolute(q_err))));
    print("max grad error at centroids: {}".format(np.amax(c_grad_err_norm)));
    print("max grad error at quadrature pts: {}".format(np.amax(q_grad_err_norm)));

    print("average error at nodes: {}".format(np.mean(np.absolute(v_err))));
    print("average error at centroids: {}".format(np.mean(np.absolute(c_err))));
    print("average error at quadrature pts: {}".format(np.mean(np.absolute(q_err))));
    print("average grad error at centroids: {}".format(np.mean(c_grad_err_norm)));
    print("average grad error at quadrature pts: {}".format(np.mean(q_grad_err_norm)));

    print("L2 error at nodes: {}".format(numpy.linalg.norm(v_err)));
    print("L2 error at centroids: {}".format(numpy.linalg.norm(c_err)));
    print("L2 error at quadrature pts: {}".format(numpy.linalg.norm(q_err)));
    print("L2 grad error at centroids: {}".format(numpy.linalg.norm(c_grad_err_norm)));
    print("L2 grad error at quadrature pts: {}".format(numpy.linalg.norm(q_grad_err_norm)));

    print("==Relative errors==");
    print("max rel error at nodes: {}".format(np.amax(np.absolute(v_rel_err))));
    print("max rel error at centroids: {}".format(np.amax(np.absolute(c_rel_err))));
    print("max rel error at quadrature pts: {}".format(np.amax(np.absolute(q_rel_err))));
    print("max grad rel error at centroids: {}".format(np.amax(c_grad_rel_err_norm)));
    print("max grad rel error at quadrature pts: {}".format(np.amax(q_grad_rel_err_norm)));

    mesh.add_attribute("target_solution");
    mesh.set_attribute("target_solution", v_values);
    mesh.add_attribute("solution");
    mesh.set_attribute("solution", sol_values);
    mesh.add_attribute("v_error");
    mesh.set_attribute("v_error", v_err);
    mesh.add_attribute("v_rel_error");
    mesh.set_attribute("v_rel_error", v_rel_err);
    mesh.add_attribute("c_error");
    mesh.set_attribute("c_error", c_err);
    mesh.add_attribute("c_rel_error");
    mesh.set_attribute("c_rel_error", c_rel_err);
    mesh.add_attribute("q_error");
    mesh.set_attribute("q_error", np.amax(np.absolute(q_err.reshape((-1,4),
        order="C")), axis=1));
    mesh.add_attribute("q_rel_error");
    mesh.set_attribute("q_rel_error", np.amax(np.absolute(q_rel_err.reshape((-1,4),
        order="C")), axis=1));
    mesh.add_attribute("c_grad_error");
    mesh.set_attribute("c_grad_error", c_grad_err_norm);
    mesh.add_attribute("c_grad_rel_error");
    mesh.set_attribute("c_grad_rel_error", c_grad_rel_err_norm);
    mesh.add_attribute("q_grad_error");
    mesh.set_attribute("q_grad_error", np.amax(q_grad_err_norm.reshape((-1,4),
        order="C"), axis=1));
    mesh.add_attribute("q_grad_rel_error");
    mesh.set_attribute("q_grad_rel_error", np.amax(q_grad_rel_err_norm.reshape((-1,4),
        order="C"), axis=1));
    mesh.add_attribute("solution_grad");
    mesh.set_attribute("solution_grad", sol_c_grad);
    mesh.add_attribute("target_grad");
    mesh.set_attribute("target_grad", c_grad);
    pymesh.save_mesh(args.output_mesh, mesh, *mesh.attribute_names);

if __name__ == "__main__":
    main();
