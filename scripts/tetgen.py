#!/usr/bin/env python
import argparse
import pymesh
import math
import numpy as np

def parse_args():
    parser = argparse.ArgumentParser(description="Run tetgen");
    parser.add_argument("-f", "--flags",
            help="tetgen arguments without the '-'", default="p");
    parser.add_argument("--auto-size", action="store_true",
            help="Compute ideal tet size automatically (beta)");
    parser.add_argument("in_mesh", help="input mesh");
    parser.add_argument("out_mesh", help="output mesh");
    args = parser.parse_args();
    return args;

def main():
    args = parse_args();
    mesh = pymesh.load_mesh(args.in_mesh);
    if (args.auto_size):
        bbox_min, bbox_max = mesh.bbox;
        target_edge_len = np.sum(bbox_max - bbox_min) / 30.0 * 4.0 / 3.0;
        target_volume = target_edge_len ** 3 * math.sqrt(2) / 12.0;
        flags = "{}a{}".format(args.flags, target_volume);
    else:
        flags = args.flags;
    tet_mesh = pymesh.tetgen(mesh, flags);
    pymesh.save_mesh(args.out_mesh, tet_mesh);

if __name__ == "__main__":
    main();

