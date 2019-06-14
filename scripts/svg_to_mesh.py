#!/usr/bin/env python

""" Convert a svg file into 2D triangle mesh.
"""

import argparse
import logging
import pymesh
import numpy as np
from numpy.linalg import norm
import os.path
from subprocess import check_call
from time import time

def parse_args():
    parser = argparse.ArgumentParser(__doc__);
    parser.add_argument("--engine", help="Triangulation engine", choices=(
                "triangle_conforming_delaunay",
                "triangle_constrained_delaunay",
                "cgal_constrained_delaunay",
                "cgal_conforming_delaunay",
                "geogram_delaunay",
                "jigsaw_frontal_delaunay",
                "mmg_delaunay", "triwild"),
            default="triangle_conforming_delaunay");
    parser.add_argument("--resolve-self-intersection", "-r", action="store_true");
    parser.add_argument("--with-frame", '-f', action="store_true");
    parser.add_argument("--with-cell-label", "-l", action="store_true");
    parser.add_argument("--with-cleanup", "-c", action="store_true");
    parser.add_argument("--with-triangulation", "-t", action="store_true");
    parser.add_argument("--stage", type=int, default=1);
    parser.add_argument("--epsilon", type=float, default=1e-3);
    parser.add_argument("--log", type=str, help="Logging level",
            choices=["DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"],
            default="INFO");
    parser.add_argument("--with-features", '-F', action="store_true",
            help="TriWild specific option to preserve features");
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

def add_frame(wires):
    if wires.num_vertices == 0:
        return wires;
    vertices = wires.vertices;
    edges = wires.edges;

    bbox_min = np.amin(vertices, axis=0);
    bbox_max = np.amax(vertices, axis=0);
    bbox_center = 0.5 * (bbox_min + bbox_max);
    diag_len = norm(bbox_max - bbox_min);
    offset = np.ones(2) * diag_len / 20;
    bbox_min -= offset;
    bbox_max += offset;

    frame_vertices = np.array([
        [bbox_min[0], bbox_min[1]],
        [bbox_max[0], bbox_min[1]],
        [bbox_max[0], bbox_max[1]],
        [bbox_min[0], bbox_max[1]],
        ]);
    frame_edges = np.array([
        [0, 1],
        [1, 2],
        [2, 3],
        [3, 0],
        ]) + wires.num_vertices;

    vertices = np.vstack([vertices, frame_vertices]);
    edges = np.vstack([edges, frame_edges]);
    wires.load(vertices, edges);
    return wires;

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

def triangulate(wires, engine, stage, eps, logger, wire_file, json_file):
    if wires.num_vertices == 0:
        return pymesh.form_mesh(np.zeros((0, 2)), np.zeros((0,3)));
    basename = os.path.splitext(wire_file)[0];
    if engine == "triwild":
        out_mesh = "{}_linear.msh".format(basename);
        log_file = "{}_triwild.log".format(basename);
        if json_file is not None:
            command = "TriWild --mute-log --feature-envelope-r {} --stage {} --log-file {} --feature-input {} --output-linear-mesh --skip-eps --input {} --output {}".format(
                    eps, stage, log_file, json_file, wire_file, basename);
        else:
            command = "TriWild --mute-log --feature-envelope-r {} --stage {} --log-file {} --output-linear-mesh --skip-eps --input {} --output {}".format(
                    eps, stage, log_file, wire_file, basename);
        print(command);
        start_time = time();
        check_call(command.split());
        finish_time = time();
        t = finish_time - start_time;
        mesh = pymesh.load_mesh(out_mesh, drop_zero_dim=True);
    else:
        mesh, t = pymesh.triangulate_beta(wires.vertices, wires.edges,
                engine=engine, with_timing=True);
    logger.info("Triangulation running time: {}".format(t));

    return mesh;

def compute_cell_labels(wires, mesh, logger):
    start_time = time();
    arrangement = pymesh.Arrangement2();
    arrangement.points = wires.vertices;
    arrangement.segments = wires.edges;
    arrangement.run();
    mesh.add_attribute("face_centroid");
    centroids = mesh.get_face_attribute("face_centroid");
    r = arrangement.query(centroids);
    finish_time = time();
    t = finish_time - start_time;
    logger.info("Arrangement running time: {}".format(t));

    cell_type = np.array([item[0] for item in r]);
    cell_ids = np.array([item[1] for item in r]);
    cell_ids[cell_type != pymesh.Arrangement2.ElementType.CELL] = -1;
    mesh.add_attribute("cell");
    mesh.set_attribute("cell", cell_ids);

def solve_heat_equation(mesh):
    cell_ids = mesh.get_attribute("cell").ravel().astype(int);
    cut_mesh = pymesh.cut_mesh(mesh, cell_ids);

    tree = pymesh.AABBTree2();
    tree.load_data(cut_mesh.vertices, cut_mesh.boundary_edges);
    sq_dist, indices = tree.look_up(mesh.vertices);

    mesh.add_attribute("sq_dist");
    mesh.set_attribute("sq_dist", sq_dist);

    return mesh;

def main():
    args = parse_args();
    logger = get_logger(args.log);
    logger.info("Triangulation engine: {}".format(args.engine));

    wires = pymesh.wires.WireNetwork.create_from_file(args.input_svg);
    wires = drop_zero_dim(wires);

    if wires.num_vertices == 0:
        logger.warn("Input is empty");

    if args.with_frame and args.engine != "triwild":
        wires = add_frame(wires);
    if args.resolve_self_intersection:
        wires = resolve_self_intersection(wires, logger);
    if args.with_cleanup:
        wires = cleanup(wires, logger);
    if args.with_features:
        json_file = "{}.json".format(os.path.splitext(args.input_svg)[0]);
        assert(os.path.exists(json_file));
    else:
        json_file = None;

    basename = os.path.splitext(args.output_mesh)[0];
    wire_file = basename + ".wire";
    wires.write_to_file(wire_file);

    if args.with_triangulation:
        if os.path.splitext(args.input_svg)[1] != ".svg" and args.engine == "triwild":
            # Avoid data loss from conversion.
            assert(not args.with_cleanup);
            assert(not args.resolve_self_intersection);
            with open(args.input_svg, 'r') as fin:
                data = fin.read();
            with open(wire_file, 'w') as fout:
                fout.write(data);
        mesh = triangulate(wires, args.engine, args.stage, args.epsilon, logger, wire_file, json_file);

        if mesh.num_vertices > 0 and args.with_cell_label:
            compute_cell_labels(wires, mesh, logger);
            mesh = solve_heat_equation(mesh);
            pymesh.save_mesh(args.output_mesh, mesh, "cell", "sq_dist");
        else:
            pymesh.save_mesh(args.output_mesh, mesh);

if __name__ == "__main__":
    main();
