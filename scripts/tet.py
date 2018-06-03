#!/usr/bin/env python
import argparse
import logging
import numpy as np
import os
import os.path
import pymesh
import sys

def parse_args():
    parser = argparse.ArgumentParser(description="Tet mesh generation");
    parser.add_argument("--engine", help="tet mesh engine",
            choices = (
                "cgal",
                "cgal_no_features",
                "cgal_implicit",
                "tetgen",
                "geogram",
                "quartet",
                "delpsc",
                "vegafem",
                "mmg",
                "tetwild"),
            default="tetgen");
    parser.add_argument("--radius-edge-ratio",
            help="Max radius to edge ratio",
            type=float, default=2.0);
    parser.add_argument("--cell-size", help="Max circumradius of tets",
            type=float, default=-1.0);
    parser.add_argument("--facet-distance",
            help="Max distance between facet's circumcenter to the center of its Delaunay ball",
            type=float, default=-1.0);
    parser.add_argument("--feature-angle",
            help="Angle threshold (in degrees) for feature extraction.",
            type=float, default=120);
    parser.add_argument("--log", type=str, help="Logging level",
            choices=["DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"],
            default="WARNING");
    parser.add_argument("in_mesh", help="Input mesh");
    parser.add_argument("out_mesh", help="Output mesh");
    args = parser.parse_args();
    return args;

def main():
    args = parse_args();

    numeric_level = getattr(logging, args.log, None);
    if not isinstance(numeric_level, int):
        raise ValueError('Invalid log level: %s' % loglevel);
    logging.basicConfig(level=numeric_level);
    logger = logging.getLogger("tet.py");

    mesh = pymesh.load_mesh(args.in_mesh);
    if mesh.vertex_per_face == 4:
        logger.info("Spliting quad mesh into triangle mesh");
        mesh = pymesh.quad_to_tri(mesh);

    tet_mesh, t = pymesh.tetrahedralize(mesh,
            args.cell_size,
            args.radius_edge_ratio,
            args.facet_distance,
            args.feature_angle,
            args.engine,
            with_timing=True);
    pymesh.save_mesh(args.out_mesh, tet_mesh);
    logger.info("Running time: {}".format(t));

if __name__ == "__main__":
    main();

