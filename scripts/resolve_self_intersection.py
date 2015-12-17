#!/usr/bin/env python

""" Resolve self-intersections in mesh.
"""

import argparse
import pymesh
import logging

def parse_args():
    parser = argparse.ArgumentParser(description=__doc__);
    parser.add_argument("--max-iterations", type=int, default=1,
            help="Max number of times of calling resolve function.");
    parser.add_argument("in_mesh", help="input mesh");
    parser.add_argument("out_mesh", help="output mesh");
    return parser.parse_args();

def main():
    args = parse_args();
    mesh = pymesh.load_mesh(args.in_mesh);
    intersecting_faces = pymesh.detect_self_intersection(mesh);

    counter = 0;
    while len(intersecting_faces) > 0 and counter < args.max_iterations:
        mesh = pymesh.resolve_self_intersection(mesh, "igl");
        mesh, __ = pymesh.remove_duplicated_faces(mesh, fins_only=True);
        intersecting_faces = pymesh.detect_self_intersection(mesh);
        counter += 1;

    if len(intersecting_faces) > 0:
        logging.warn("Resolving failed: max iteration reached!");

    pymesh.save_mesh(args.out_mesh, mesh);

if __name__ == "__main__":
    main();
