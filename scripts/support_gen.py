#!/usr/bin/env python

"""
Generate support for a given wire network.
"""

import argparse
import numpy as np
import os.path

import pymesh
from pymesh.meshutils import generate_box_mesh
from pymesh.misc import Quaternion
from pymesh.meshio import form_mesh
from pymesh.wires import WireNetwork, Inflator

def load_wire(wire_file):
    network = WireNetwork();
    network.load_from_file(wire_file);
    return network;

def inflate_supports(support_wires):
    thickness = support_wires.get_attribute("thickness");

    inflator = Inflator(support_wires);
    inflator.inflate(thickness, per_vertex_thickness=True);
    mesh = inflator.mesh;
    return mesh;

def add_base(support_mesh, print_dir, support_length):
    Z_dir = np.array([0.0, 0.0, 1.0]);
    vertices = support_mesh.vertices.reshape((-1, 3), order="C");
    faces = support_mesh.faces.reshape((-1, 3), order="C");

    height = np.dot(print_dir, vertices.T).ravel();
    projection = vertices - np.outer(height, print_dir);

    if np.all(print_dir == Z_dir):
        rot = Quaternion().to_matrix();
    else:
        rot = Quaternion.fromData(print_dir, Z_dir).to_matrix();
    projection = np.dot(rot, projection.T);
    bbox_min = np.amin(projection, axis=1) - Z_dir * 0.15;
    bbox_max = np.amax(projection, axis=1) + Z_dir * 0.15;
    base = generate_box_mesh(bbox_min, bbox_max);
    base_vertices = np.dot(rot.T, base.vertices.T).T\
            + print_dir * np.amin(height);

    num_support_vertices = support_mesh.num_vertices;
    vertices = np.vstack([vertices, base_vertices]);
    faces = np.vstack([faces, base.faces + num_support_vertices]);

    return form_mesh(vertices, faces);

def generate_supports(wire_network, print_dir, support_length,
        support_thickness, tol):
    min_height = np.amin(np.dot(wire_network.vertices, print_dir));
    proj_dist = [];
    vertices = [];
    edges = [];
    for i,v in enumerate(wire_network.vertices):
        neighbors = wire_network.vertices[wire_network.get_vertex_neighbors(i)];
        neighbors = neighbors.reshape((-1, 3), order="C");
        h = np.dot(v, print_dir);
        if np.any(np.dot(neighbors, print_dir) <= h + tol) and h > min_height + tol:
            continue;
        vertices.append(v);
        proj_dist.append(h);

    num_supports = len(vertices);
    base_vertices_top_end = vertices - print_dir * support_thickness;
    base_vertices_bottom_end = vertices - np.outer(
            (proj_dist - min_height + support_length), print_dir);
    vertices = np.vstack((vertices + 0.05 * print_dir,
        base_vertices_top_end, base_vertices_bottom_end));
    edges = np.vstack((
            np.arange(num_supports * 2).reshape((num_supports, 2), order="F"),
            np.arange(num_supports, num_supports * 3).reshape((num_supports, 2), order="F")));
    thickness = np.hstack((
            np.ones(num_supports) * support_thickness * 0.5,
            np.ones(num_supports) * support_thickness,
            np.ones(num_supports) * support_thickness,));

    support_wires = WireNetwork();
    support_wires.load(vertices, edges);
    support_wires.add_attribute("thickness", thickness);
    return support_wires;

def parse_args():
    parser = argparse.ArgumentParser(
            description="Wire mesh support generation");
    parser.add_argument("--print-dir", choices=["X", "Y", "Z", "-X", "-Y", "-Z"], default="Z");
    parser.add_argument("--length", "-l", default=3.0, type=float,
            help="Support minimum length in mm");
    parser.add_argument("--thickness", "-t", default=0.5, type=float,
            help="Support thickness in mm");
    parser.add_argument("--support-tol", "-s", type=float, default=1e-6,
            help="Support tolerance in mm");
    parser.add_argument("input_file");
    parser.add_argument("output_mesh");
    return parser.parse_args();

def main():
    args = parse_args();
    print("Generating support in {} direction of thickness {} and length {}"\
            .format(args.print_dir, args.thickness, args.length));
    wire_network = load_wire(args.input_file);
    direction = {
            "X": np.array([1, 0, 0], dtype=float),
            "Y": np.array([0, 1, 0], dtype=float),
            "Z": np.array([0, 0, 1], dtype=float),
            "-X": np.array([-1, 0, 0], dtype=float),
            "-Y": np.array([0, -1, 0], dtype=float),
            "-Z": np.array([0, 0, -1], dtype=float) };
    print_dir = direction[args.print_dir];
    support_wires = generate_supports(wire_network,
            print_dir, args.length, args.thickness, args.support_tol);
    support_mesh = inflate_supports(support_wires);
    support_mesh = add_base(support_mesh, print_dir, args.length);
    pymesh.save_mesh(args.output_mesh, support_mesh);

if __name__ == "__main__":
    main();
