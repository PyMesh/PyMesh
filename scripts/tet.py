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
                "vegafem"),
            default="tetgen");
    parser.add_argument("--radius-edge-ratio",
            help="Max radius to edge ratio",
            type=float, default=2.0);
    parser.add_argument("--cell-size", help="max circumradius of tets",
            type=float, default=-1.0);
    parser.add_argument("--log", type=str, help="Logging level",
            choices=["DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"],
            default="WARNING");
    parser.add_argument("in_mesh", help="input mesh");
    parser.add_argument("out_mesh", help="output mesh");
    args = parser.parse_args();
    return args;

def main():
    args = parse_args();

    numeric_level = getattr(logging, args.log, None);
    if not isinstance(numeric_level, int):
        raise ValueError('Invalid log level: %s' % loglevel);
    logging.basicConfig(level=numeric_level);

    mesh = pymesh.load_mesh(args.in_mesh);

    tet_mesh = pymesh.tetrahedralize(mesh,
            args.cell_size,
            args.radius_edge_ratio,
            args.engine);
    pymesh.save_mesh(args.out_mesh, tet_mesh);

if __name__ == "__main__":
    main();

