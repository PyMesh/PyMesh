#!/usr/bin/env python
# -*- coding: utf-8 -*-

""" Inflate input svg file in repoussé-like style.
"""

import argparse
import logging
import pymesh
import math
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
    parser.add_argument("--repousse", action="store_true",
            help="Inflate the surface in repoussé-like style");
    parser.add_argument("--bevel", type=float, default=0.0,
            help="Bevel edges by this amount");
    parser.add_argument("--extrude", type=float, default=0.0,
            help="Extrude input graphics");
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

def drop_frame(wires):
    vertices = wires.vertices[4:,];
    edges = wires.edges[4:,] - 4;
    wires.load(vertices, edges);
    return wires;

def flip_orientation(wires):
    edges = wires.edges[:,[1,0]];
    wires.load(wires.vertices, edges);
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

def uniform_sampling(wires):
    bbox_min, bbox_max = wires.bbox;
    tol = norm(bbox_max - bbox_min) / 1000;

    wire_lengths = wires.wire_lengths;
    vertices = wires.vertices;
    edges = wires.edges;

    vertex_count = wires.num_vertices;
    sampled_vertices = [wires.vertices];
    sampled_edges = [];
    for i in range(wires.num_edges):
        if wire_lengths[i] <= tol:
            sampled_edges.append(edges[i]);
            continue;
        v0 = vertices[edges[i,0]];
        v1 = vertices[edges[i,1]];
        num_samples = math.ceil(wire_lengths[i] / tol);
        samples = np.array([
            v0 * (1-s/num_samples) + v1 * s/num_samples
            for s in range(1,int(num_samples)) ]);
        new_edges = [[vertex_count + j-1, vertex_count + j]
                for j in range(int(num_samples))];
        new_edges[0][0] = edges[i,0];
        new_edges[-1][1] = edges[i,1];

        sampled_vertices.append(samples);
        sampled_edges.append(np.array(new_edges));

        vertex_count += num_samples-1;

    wires.load(np.vstack(sampled_vertices), np.vstack(sampled_edges));
    return wires;

def conforming_triangulate(wires, logger, auto_hole_detection):
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

def constrained_triangulate(wires, logger, auto_hole_detection):
    bbox_min, bbox_max = wires.bbox;
    tol = norm(bbox_max - bbox_min) / 20;

    engine = pymesh.triangle();
    engine.points = wires.vertices;
    engine.segments = wires.edges;
    engine.conforming_delaunay = False;
    engine.max_num_steiner_points = 0;
    engine.split_boundary = False; 
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

def extract_straight_skeleton(wires, logger, remove_holes):
    wires = uniform_sampling(wires);
    mesh = constrained_triangulate(wires, logger, remove_holes);

    mesh = pymesh.subdivide(mesh, order=1, method="simple");
    bd_vertices = mesh.boundary_vertices;
    on_skeleton = np.ones(mesh.num_vertices, dtype=bool);
    on_skeleton[bd_vertices] = False;

    vertices = mesh.vertices;
    faces = mesh.faces;
    junction_triangles = np.all(on_skeleton[faces], axis=1)
    junction_centers = np.mean(vertices[faces[junction_triangles,:],:], axis=1);

    #skeleton_samples = np.vstack(
    #        (vertices[on_skeleton,:], junction_centers));
    skeleton_samples = vertices[on_skeleton, :];

    vertices = np.vstack((wires.vertices, skeleton_samples));
    wires.load(vertices, wires.edges);
    mesh = constrained_triangulate(wires, logger, remove_holes);

    return mesh;

def bevel(wires, logger, remove_holes, dist):
    bbox_min, bbox_max = wires.bbox;

    #wires = uniform_sampling(wires);
    mesh = constrained_triangulate(wires, logger, remove_holes);

    cell_ids = mesh.get_attribute("cell").ravel().astype(int);
    num_cells = np.amax(cell_ids) + 1;

    comps = [];
    for i in range(num_cells):
        to_keep = np.arange(mesh.num_faces, dtype=int)[cell_ids == i];
        if not np.any(to_keep):
            continue;

        cut_mesh = pymesh.submesh(mesh, to_keep, 0);
        bd_edges = cut_mesh.boundary_edges;
        vertices, edges, __ = pymesh.remove_isolated_vertices_raw(
                cut_mesh.vertices, bd_edges);
        bd_wires = pymesh.wires.WireNetwork.create_from_data(vertices, edges);

        offset_dir = np.zeros((bd_wires.num_vertices, 2));
        for ei in edges:
            v0 = ei[0];
            v1 = ei[1];
            adj_vts = bd_wires.get_vertex_neighbors(v0);
            if len(adj_vts) == 2:
                if adj_vts[0] == v1:
                    vp = adj_vts[1];
                else:
                    vp = adj_vts[0];
                e0 = vertices[v1] - vertices[v0];
                e1 = vertices[vp] - vertices[v0];
                e0 /= norm(e0);
                e1 /= norm(e1);
                theta = math.atan2(e0[0]*e1[1] - e0[1]*e1[0], e0.dot(e1));

                if abs(theta) > 0.99 * math.pi:
                    offset = np.array([-e0[1], e0[0]]);
                    scale = 1.0;
                else:
                    if theta > 0:
                        offset = e0 + e1;
                        scale = math.sin(theta/2);
                    else:
                        offset = -e0 - e1
                        scale = math.cos(math.pi/2 + theta/2);
                    offset /= norm(offset);
                offset_dir[v0] = offset * dist / scale;

        offset_vertices = vertices + offset_dir;
        vertices = np.vstack((vertices, offset_vertices));
        edges = np.vstack((edges, edges + bd_wires.num_vertices));

        vertices, edges, __ = pymesh.remove_duplicated_vertices_raw(
                vertices, edges, dist/2);

        comp_wires = pymesh.wires.WireNetwork.create_from_data(vertices, edges);
        comp = constrained_triangulate(comp_wires, logger, True);
        comps.append(comp);

    mesh = pymesh.merge_meshes(comps);
    bd_vertices = mesh.boundary_vertices;
    is_inside = np.ones(mesh.num_vertices, dtype=bool);
    is_inside[bd_vertices] = False;

    vertices = np.hstack((mesh.vertices, np.zeros((mesh.num_vertices, 1))));
    vertices[is_inside, 2] = dist;
    mesh = pymesh.form_mesh(vertices, mesh.faces);
    return mesh;

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

        L_rhs = M * np.ones(dof) * -0.5;
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

    vertices = mesh.vertices[:,:2];
    mesh_2d = pymesh.form_mesh(vertices, mesh.faces);
    pymesh.save_mesh("out_2d.msh", mesh_2d)
    return mesh;

def extrude(mesh, depth, logger):
    start_time = time();
    num_vertices = mesh.num_vertices;
    num_faces = mesh.num_faces;

    top_vertices = mesh.vertices;
    top_faces = mesh.faces;

    if mesh.dim == 2:
        top_vertices = np.hstack((top_vertices, np.zeros((mesh.num_vertices, 1))));

    bottom_vertices = np.copy(top_vertices);
    bottom_vertices[:,2] -= depth;
    bottom_faces = top_faces[:,[0,2,1]] + num_vertices;

    bd_edges = mesh.boundary_edges;
    bd_faces = np.array([
        [e[1], e[0], e[0]+num_vertices,
         e[0]+num_vertices, e[1]+num_vertices, e[1] ]
        for e in bd_edges]).reshape((-1, 3), order="C");

    vertices = np.vstack([top_vertices, bottom_vertices]);
    faces = np.vstack([top_faces, bottom_faces, bd_faces]);

    finish_time = time();
    t = finish_time - start_time;
    logger.info("Extrusion running time: {}".format(t));

    return pymesh.form_mesh(vertices, faces);

def main():
    args = parse_args();
    logger = get_logger(args.log);
    wires = pymesh.wires.WireNetwork.create_from_file(args.input_svg);
    wires = drop_zero_dim(wires);
    wires = drop_frame(wires);
    wires = flip_orientation(wires);
    wires = resolve_self_intersection(wires, logger);
    wires = cleanup(wires, logger);

    if args.repousse:
        mesh = conforming_triangulate(wires, logger, args.remove_holes);
        mesh = repousse(mesh, logger);
    elif args.bevel > 0:
        mesh = bevel(wires, logger, args.remove_holes, args.bevel);
    else:
        mesh = constrained_triangulate(wires, logger, args.remove_holes);

    if args.extrude > 0:
        mesh = extrude(mesh, args.extrude, logger);

    pymesh.save_mesh(args.output_mesh, mesh);

if __name__ == "__main__":
    main();

