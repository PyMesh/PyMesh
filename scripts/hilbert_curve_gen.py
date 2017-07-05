#!/usr/bin/env python
"""
Generate hilbert curve at a set resolution.
"""

import argparse
import numpy as np
import pymesh

def parse_args():
    parser = argparse.ArgumentParser(__doc__);
    parser.add_argument("--resolution", "-r", type=int, help="resolution",
            default=1);
    parser.add_argument("output_mesh");
    return parser.parse_args();

def main():
    args = parse_args();
    #  Config 0     Config 1     Config 2     Config 3
    # 1 +----+ 2   3 +----+ 2   1 +----+ 0   3 +    + 0
    #   |    |            |       |            |    |
    # 0 +    + 3   0 +----+ 1   2 +----+ 3   2 +----+ 1
    configs = np.array([
            [[0, 3],[1, 2]],
            [[0, 1],[3, 2]],
            [[2, 3],[1, 0]],
            [[2, 1],[3, 0]] ], dtype=int);
    config_transform = np.array([
            [1, 0, 0, 2],
            [0, 1, 1, 3],
            [3, 2, 2, 0],
            [2, 3, 3, 1] ], dtype=int);

    pts = np.zeros((4**args.resolution, 2));

    for x in range(2**args.resolution):
        for y in range(2**args.resolution):
            X = x;
            Y = y;
            p = np.copy([x,y]);
            quadrant = 0;
            index = 0;
            for order in range(args.resolution):
                exp = args.resolution-order-1;
                mask = 1 << exp;
                step = 2**(args.resolution-order-1);
                i = (X & mask) >> exp;
                j = (Y & mask) >> exp;
                local_index = configs[quadrant][i][j];
                quadrant = config_transform[quadrant][local_index];
                index += local_index * step*step;
                X >> 1;
                Y >> 1;
            pts[index,:] = p;

    vertices = np.array(pts);
    edges = np.array([[i, i+1] for i in range(4**args.resolution - 1)]);
    wires = pymesh.wires.WireNetwork.create_from_data(vertices, edges);
    wires.write_to_file("tmp.obj");
    inflator = pymesh.wires.Inflator(wires);
    inflator.inflate(0.2);

    mesh = inflator.mesh;
    pymesh.save_mesh(args.output_mesh, mesh);

if __name__ == "__main__":
    main();
