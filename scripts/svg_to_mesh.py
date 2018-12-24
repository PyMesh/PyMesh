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
    parser.add_argument("--log", type=str, help="Logging level",
            choices=["DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"],
            default="INFO");
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
    bbox_min, bbox_max = wires.bbox;
    tol = norm(bbox_max - bbox_min) / 1000;
    start_time = time();
    vertices, edges = pymesh.snap_rounding(wires.vertices, wires.edges, tol);
    finish_time = time();
    t = finish_time - start_time;
    logger.info("Snap rounding running time: {}".format(t));
    wires.load(vertices, edges);
    return wires;

def triangulate(wires, engine, logger, wire_file):
    basename = os.path.splitext(wire_file)[0];
    if engine == "triwild":
        out_mesh = "{}.stl".format(basename);
        log_file = "{}_triwild.log".format(basename);
        command = "TriWild --choice TRI --input {} --output {}".format(
                wire_file, out_mesh);
        with open(log_file, 'w') as fout:
            start_time = time();
            check_call(command.split(), stdout=fout);
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

def main():
    args = parse_args();
    logger = get_logger(args.log);
    logger.info("Triangulation engine: {}".format(args.engine));

    wires = pymesh.wires.WireNetwork.create_from_file(args.input_svg);
    wires = drop_zero_dim(wires);

    if args.with_frame and args.engine != "triwild":
        wires = add_frame(wires);
    if args.resolve_self_intersection:
        wires = resolve_self_intersection(wires, logger);
    if args.with_cleanup:
        wires = cleanup(wires, logger);

    basename = os.path.splitext(args.output_mesh)[0];
    wire_file = basename + ".wire";
    wires.write_to_file(wire_file);

    if args.with_triangulation:
        mesh = triangulate(wires, args.engine, logger, wire_file);

        if args.with_cell_label:
            compute_cell_labels(wires, mesh, logger);
            pymesh.save_mesh(args.output_mesh, mesh, "cell");
        else:
            pymesh.save_mesh(args.output_mesh, mesh);

if __name__ == "__main__":
    main();
