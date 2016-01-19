#!/usr/bin/env python

""" Compute Minkowski sum of a mesh with a path
"""

import argparse
import pymesh
import pymesh.wires
import numpy as np
import os.path

def parse_args():
    parser = argparse.ArgumentParser(description=__doc__);
    parser.add_argument("input_mesh");
    parser.add_argument("path");
    parser.add_argument("output_mesh");
    return parser.parse_args();

def load_path_file(filename):
    path = [];
    with open(filename, 'r') as fin:
        for line in fin:
            fields = line.split();
            if len(fields) == 0 or line[0] == "#":
                continue;
            path.append([float(x) for x in fields]);
    return np.array(path, dtype=float);

def chain_wires(wires):
    assert(wires.num_vertices > 0);
    visited = np.zeros(wires.num_vertices, dtype=bool);
    path = [0];
    visited[0] = True;
    while not np.all(visited):
        front = path[0];
        front_neighbors = wires.get_vertex_neighbors(int(front));
        for v in front_neighbors:
            if visited[v]: continue;
            path.insert(0, v);
            visited[v] = True;
            break;
        end = path[-1];
        end_neighbors = wires.get_vertex_neighbors(int(end));
        for v in end_neighbors:
            if visited[v]: continue;
            visited[v] = True;
            path.append(v);
            break;

    first_neighbors = wires.get_vertex_neighbors(int(path[0])).squeeze();
    if len(path) > 2 and path[-1] in first_neighbors:
        # Close the loop.
        path.append(path[0]);

    path = wires.vertices[path];
    return path;

def main():
    args = parse_args();
    mesh = pymesh.load_mesh(args.input_mesh);
    wires = pymesh.wires.WireNetwork.create_from_file(args.path);
    path = chain_wires(wires);

    result = pymesh.minkowski_sum(mesh, path);
    pymesh.save_mesh(args.output_mesh, result);

if __name__ == "__main__":
    main();
