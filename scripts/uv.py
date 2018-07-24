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
    parser.add_argument("--separate", action="store_true",
            help="Segment the 3D model based on uv charts.  Topology may not be preserved");
    parser.add_argument("--cut", action="store_true",
            help="Cut the 3D mesh based on uv patches.  Topology is preserved.");
    parser.add_argument("input_mesh", help="input mesh");
    parser.add_argument("output_mesh", help="output mesh");
    return parser.parse_args();

def main():
    args = parse_args();
    mesh = pymesh.load_mesh(args.input_mesh);
    if not mesh.has_attribute("corner_texture"):
        raise RuntimeError("Mesh contains no uv!");
    mesh.add_attribute("face_area");

    uv = mesh.get_attribute("corner_texture").reshape((-1, 2));
    if len(uv) == 0:
        raise RuntimeError("Invalid uv size.");

    faces = np.arange(mesh.num_faces * mesh.vertex_per_face).reshape(
            (-1, mesh.vertex_per_face));
    uv_mesh = pymesh.form_mesh(uv, faces);
    uv_mesh.add_attribute("face_area");

    ori_area = mesh.get_face_attribute("face_area");
    uv_area = uv_mesh.get_face_attribute("face_area");
    area_ratio = np.divide(uv_area, ori_area);

    uv_mesh.add_attribute("area_ratio");
    uv_mesh.set_attribute("area_ratio", area_ratio);
    mesh.add_attribute("area_ratio");
    mesh.set_attribute("area_ratio", area_ratio);
    mesh.add_attribute("u");
    mesh.set_attribute("u", uv[:,0]);

    if (args.separate):
        uv_mesh, info = pymesh.remove_duplicated_vertices(uv_mesh);
        comps = pymesh.separate_mesh(uv_mesh);
        segments = [];
        uv = uv.reshape((-1, 6), order="C");
        vertices = mesh.vertices;
        combined_uv = [];
        for comp in comps:
            ori_vertex_indices = comp.get_attribute("ori_vertex_index").ravel();
            ori_elem_indices = comp.get_attribute("ori_elem_index").ravel().astype(int);

            segment = pymesh.submesh(mesh, ori_elem_indices, 0);
            ori_face_indices = segment.get_attribute("ori_face_index").ravel().astype(int);
            ori_uv = uv[ori_face_indices];
            combined_uv.append(ori_uv);
            segment.add_attribute("corner_texture");
            segment.set_attribute("corner_texture", ori_uv.ravel());
            segments.append(segment);

        combined_uv = np.vstack(combined_uv);
        mesh = pymesh.merge_meshes(segments);
        mesh.add_attribute("corner_texture");
        mesh.set_attribute("corner_texture", combined_uv.ravel(order="C"));
    elif args.cut:
        uv_mesh, info = pymesh.remove_duplicated_vertices(uv_mesh);
        index_map = info["index_map"];
        vertices = mesh.vertices;
        faces = mesh.faces;
        vertices = vertices[faces.ravel(order="C")];
        new_vertices = np.zeros((uv_mesh.num_vertices, 3));
        new_vertices[index_map] = vertices;

        mesh = pymesh.form_mesh(new_vertices, uv_mesh.faces);
        mesh.add_attribute("corner_texture");
        mesh.set_attribute("corner_texture", uv);

    if args.save_uv:
        pymesh.save_mesh(args.output_mesh, uv_mesh, *uv_mesh.attribute_names);
    else:
        pymesh.save_mesh(args.output_mesh, mesh, *mesh.attribute_names);

if __name__ == "__main__":
    main();
