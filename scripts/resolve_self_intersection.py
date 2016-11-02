#!/usr/bin/env python

""" Resolve self-intersections in mesh.
"""

import argparse
import pymesh
import logging
import numpy as np

import tempfile
import os.path

def parse_args():
    parser = argparse.ArgumentParser(description=__doc__);
    parser.add_argument("--max-iterations", type=int, default=1,
            help="Max number of times of calling resolve function.");
    parser.add_argument("--with-rounding", action="store_true",
            help="Agressive rounding to avoid self-intersection");
    parser.add_argument("--precision", type=int, default=12,
            help="Number of decimal digits to round to");
    parser.add_argument("in_mesh", help="input mesh");
    parser.add_argument("out_mesh", help="output mesh");
    return parser.parse_args();

def main():
    args = parse_args();
    mesh = pymesh.load_mesh(args.in_mesh);
    if (args.with_rounding):
        mesh = pymesh.form_mesh(
                np.round(mesh.vertices, args.precision),
                mesh.faces);
    intersecting_faces = pymesh.detect_self_intersection(mesh);

    counter = 0;
    while len(intersecting_faces) > 0 and counter < args.max_iterations:
        if (args.with_rounding):
            involved_vertices = np.unique(mesh.faces[intersecting_faces].ravel());
            mesh.vertices_ref[involved_vertices, :] =\
                    np.round(mesh.vertices[involved_vertices, :],
                            args.precision//2);
        mesh = pymesh.resolve_self_intersection(mesh, "igl");
        mesh, __ = pymesh.remove_duplicated_faces(mesh, fins_only=True);
        if (args.with_rounding):
            mesh = pymesh.form_mesh(
                    np.round(mesh.vertices, args.precision),
                    mesh.faces);
        intersecting_faces = pymesh.detect_self_intersection(mesh);
        counter += 1;

    if len(intersecting_faces) > 0:
        logging.warn("Resolving failed: max iteration reached!");

    pymesh.save_mesh(args.out_mesh, mesh);

if __name__ == "__main__":
    main();
