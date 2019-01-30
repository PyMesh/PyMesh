#!/usr/bin/env python

"""
Extract UV out of mesh.
"""

import argparse
import pymesh
import numpy as np

def parse_args():
    parser = argparse.ArgumentParser(
            description=__doc__);
    parser.add_argument("--save-uv", action="store_true");
    parser.add_argument("input_mesh", help="input mesh");
    parser.add_argument("output_mesh", help="output mesh");
    return parser.parse_args();

def main():
    args = parse_args();
    mesh = pymesh.load_mesh(args.input_mesh);
    if not mesh.has_attribute("corner_texture"):
        raise RuntimeError("Mesh contains no uv!");

    mesh.add_attribute("face_area");
    cutted_mesh = pymesh.cut_mesh(mesh);

    uvs = cutted_mesh.get_attribute("corner_texture").reshape((-1, 2));
    faces = cutted_mesh.faces;
    per_vertex_uv = np.ndarray((cutted_mesh.num_vertices, 2));
    per_vertex_uv[faces.ravel(order="C")] = uvs;

    if not args.save_uv:
        cutted_mesh.add_attribute("u");
        cutted_mesh.set_attribute("u", per_vertex_uv[:,0]);
        cutted_mesh.add_attribute("v");
        cutted_mesh.set_attribute("v", per_vertex_uv[:,1]);
        pymesh.save_mesh(args.output_mesh, cutted_mesh, "u", "v");
    else:
        uv_mesh = pymesh.form_mesh(per_vertex_uv, faces);
        pymesh.save_mesh(args.output_mesh, uv_mesh);

if __name__ == "__main__":
    main();
