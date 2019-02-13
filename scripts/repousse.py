#!/usr/bin/env python
# -*- coding: utf-8 -*-

""" Inflate input svg file in repoussé-like style.
"""

import argparse
import logging
import pymesh
import numpy as np
from numpy.linalg import norm
import os.path
from subprocess import check_call
from time import time
import scipy.sparse

def parse_args():
    parser = argparse.ArgumentParser(__doc__);
    parser.add_argument("--log", type=str, help="Logging level",
            choices=["DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"],
            default="INFO");
    parser.add_argument("--remove-holes", action="store_true",
            help="Remove triangles on the outside of the shape");
    parser.add_argument("input_svg");
    parser.add_argument("output_mesh");
    return parser.parse_args();

def get_logger(level):
    numeric_level = getattr(logging, level, None);
    if not isinstance(numeric_level, int):
        raise ValueError('Invalid log level: {}'.format(level));
    logging.basicConfig(level=numeric_level);
    return logging.getLogger("PyMesh.Triangulation");

def drop_zero_dim(wires):
    # Trim zero dimension.
    if wires.dim == 3:
        vertices = wires.vertices;
        assert(np.all(vertices[:,2] == 0));
        vertices = vertices[:, [0,1]];
        wires.load(vertices, wires.edges);
    return wires;

def cleanup(wires, logger):
    if wires.num_vertices == 0:
        return wires;
    start_time = time();
    tol = 1e-6;
    vertices, edges, __ = pymesh.remove_duplicated_vertices_raw(
            wires.vertices, wires.edges, tol);

    # Remove duplicated edges.
    ordered_edges = np.sort(edges, axis=1);
    __, unique_edge_ids, __ = pymesh.unique_rows(ordered_edges);
    edges = edges[unique_edge_ids, :];
    wires.load(vertices, edges);

    # Remove topologically degenerate edges.
    is_not_topologically_degenerate = edges[:,0] != edges[:,1];
    if not np.all(is_not_topologically_degenerate):
        wires.filter_edges(is_not_topologically_degenerate);

    finish_time = time();
    t = finish_time - start_time;
    logger.info("Cleanup running time: {}".format(t));

    return wires;

def triangulate(wires, logger, auto_hole_detection):
    bbox_min, bbox_max = wires.bbox;
    tol = norm(bbox_max - bbox_min) / 20;

    engine = pymesh.triangle();
    engine.points = wires.vertices;
    engine.segments = wires.edges;
    engine.conforming_delaunay = True;
    engine.max_area = tol**2;
    engine.verbosity = 0;
    engine.auto_hole_detection = auto_hole_detection;

    start_time = time();
    engine.run();
    finish_time = time();

    t = finish_time - start_time;
    logger.info("Triangle running time: {}".format(t));

    mesh = engine.mesh;
    mesh.add_attribute("cell");
    mesh.set_attribute("cell", engine.regions);
    return mesh;

def resolve_self_intersection(wires, logger):
    if wires.num_vertices == 0:
        return wires;
    bbox_min, bbox_max = wires.bbox;
    tol = norm(bbox_max - bbox_min) / 1000;
    start_time = time();
    vertices, edges = pymesh.snap_rounding(wires.vertices, wires.edges, tol);
    finish_time = time();
    t = finish_time - start_time;
    logger.info("Snap rounding running time: {}".format(t));
    wires.load(vertices, edges);
    return wires;

def repousse_all(mesh, logger):
    cell_ids = mesh.get_attribute("cell").ravel().astype(int);
    mesh.add_attribute("edge_length");
    tol = np.amax(mesh.get_attribute("edge_length")) * 0.1;

    out_mesh, info = pymesh.remove_degenerated_triangles(mesh, 100);
    cell_ids = cell_ids[info["ori_face_indices"]].ravel();
    mesh, info = pymesh.split_long_edges(out_mesh, tol);
    cell_ids = cell_ids[info["ori_face_indices"]].ravel();

    mesh.enable_connectivity();
    is_border = [len(np.unique(cell_ids[mesh.get_vertex_adjacent_faces(vi)])) > 1
            for vi in range(mesh.num_vertices)];

    start_time = time();
    dof = mesh.num_vertices;
    assembler = pymesh.Assembler(mesh);
    L = assembler.assemble("laplacian");
    M = assembler.assemble("mass");

    L_rhs = M * np.ones(dof) * -1 * 1e-1;
    bd_indices = np.arange(mesh.num_vertices, dtype=int)[is_border];
    n = len(bd_indices);
    C = scipy.sparse.coo_matrix((np.ones(n),
        (np.arange(n, dtype=int), bd_indices)), shape=(n, dof));
    C_rhs = np.zeros(n);

    A = scipy.sparse.bmat([
        [-L, C.T],
        [C, None]
        ]);
    rhs = np.concatenate((L_rhs.ravel(), C_rhs));

    solver = pymesh.SparseSolver.create("SparseLU");
    solver.compute(A);
    x = solver.solve(rhs);
    z = x[:dof].reshape((-1, 1));
    vertices = np.hstack((mesh.vertices, z));

    finish_time = time();
    t = finish_time - start_time;
    logger.info("Repousse running time: {}".format(t));

    return pymesh.form_mesh(vertices, mesh.faces);

def repousse(mesh, logger):
    cell_ids = mesh.get_attribute("cell").ravel().astype(int);
    mesh.add_attribute("edge_length");
    tol = np.amax(mesh.get_attribute("edge_length")) * 0.1;

    bbox_min, bbox_max = mesh.bbox;
    scaling = 2.0 / norm(bbox_max - bbox_min);

    start_time = time();
    num_cells = np.amax(cell_ids)+1;
    results = [];
    for i in range(num_cells):
        to_keep = np.arange(mesh.num_faces, dtype=int)[cell_ids == i];
        if not np.any(to_keep):
            continue;

        cut_mesh = pymesh.submesh(mesh, to_keep, 0);
        pymesh.save_mesh("debug.msh", cut_mesh);
        cut_mesh, __ = pymesh.remove_degenerated_triangles(cut_mesh, 100);
        cut_mesh, __ = pymesh.split_long_edges(cut_mesh, tol);

        dof = cut_mesh.num_vertices;
        assembler = pymesh.Assembler(cut_mesh);
        L = assembler.assemble("laplacian");
        M = assembler.assemble("mass");

        L_rhs = M * np.ones(dof) * -0.1;
        bd_indices = cut_mesh.boundary_vertices;
        n = len(bd_indices);
        C = scipy.sparse.coo_matrix((np.ones(n),
            (np.arange(n, dtype=int), bd_indices)), shape=(n, dof));
        C_rhs = np.zeros(n);

        A = scipy.sparse.bmat([
            [-L, C.T],
            [C, None]
            ]);
        rhs = np.concatenate((L_rhs.ravel(), C_rhs));

        solver = pymesh.SparseSolver.create("SparseLU");
        solver.compute(A);
        x = solver.solve(rhs);

        z = x[:dof].reshape((-1, 1));

        vertices = np.hstack((cut_mesh.vertices, z));
        out_mesh = pymesh.form_mesh(vertices, cut_mesh.faces);
        results.append(out_mesh);

    finish_time = time();
    t = finish_time - start_time;
    logger.info("Repousse running time: {}".format(t));

    mesh = pymesh.merge_meshes(results);
    return mesh;

def main():
    args = parse_args();
    logger = get_logger(args.log);
    wires = pymesh.wires.WireNetwork.create_from_file(args.input_svg);
    wires = drop_zero_dim(wires);
    wires = resolve_self_intersection(wires, logger);
    wires = cleanup(wires, logger);

    mesh = triangulate(wires, logger, args.remove_holes);
    pymesh.save_mesh("debug.msh", mesh);
    mesh = repousse(mesh, logger);

    pymesh.save_mesh(args.output_mesh, mesh);

if __name__ == "__main__":
    main();

