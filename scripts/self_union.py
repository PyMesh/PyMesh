#!/usr/bin/env python

"""
Resolve self-intersection and nested shells using self-union.
"""

import argparse
import logging
import json
import numpy as np
import os
import os.path
import pymesh

def update_info(output_file, running_time):
    name,ext = os.path.splitext(output_file);
    info_file = name + ".info";
    if os.path.exists(info_file):
        with open(info_file, 'r') as fin:
            info = json.load(fin);
    else:
        info = {};

    info = {"running_time": running_time};
    with open(info_file, 'w') as fout:
        json.dump(info, fout, indent=4);

def parse_args():
    parser = argparse.ArgumentParser(description=__doc__);
    parser.add_argument("--engine",
            choices=["igl", "carve", "cgal", "cork", "corefinement",
                "quick_csg"],
            default="igl", help="Boolean engine to use");
    parser.add_argument("--timing", "-t",
            action="store_true", help="Report timing info");
    parser.add_argument("input_mesh", help="Input mesh");
    parser.add_argument("output_mesh", help="Output mesh");
    return parser.parse_args();

def main():
    args = parse_args();
    mesh = pymesh.load_mesh(args.input_mesh);
    if mesh.vertex_per_face == 4:
        logging.warning("Converting quad mesh to triangle mesh.");
        mesh = pymesh.quad_to_tri(mesh);

    if mesh.num_vertices ==0 or mesh.num_faces == 0:
        # Empty input mesh, output empty mesh as well.
        result = pymesh.form_mesh(np.zeros((0,3),dtype=float),
                np.zeros((0,3),dtype=int));
    else:
        bbox = mesh.bbox;
        center = (bbox[0] + bbox[1]) * 0.5;
        box = pymesh.generate_box_mesh(
                bbox[0] - np.ones(mesh.dim),
                bbox[1] + np.ones(mesh.dim));
        r = pymesh.boolean(
                mesh, box, "intersection", engine=args.engine,
                with_timing = args.timing);
        if args.timing:
            result, timing = r;
            update_info(args.output_mesh, timing);
        else:
            result = r;
    pymesh.save_mesh(args.output_mesh, result);

if __name__ == "__main__":
    main();
