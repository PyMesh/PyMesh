#!/usr/bin/env python

""" Remove nan from vertex coordinates and uv coordinates
"""

import argparse
import pymesh
import numpy as np

def parse_args():
    parser = argparse.ArgumentParser(__doc__);
    parser.add_argument("input_mesh");
    parser.add_argument("output_mesh");
    return parser.parse_args();

def main():
    args = parse_args();
    mesh = pymesh.load_mesh(args.input_mesh);
    assert(mesh.has_attribute("corner_texture"));

    mesh.enable_connectivity();
    vertices = np.copy(mesh.vertices);

    bad_vertex = np.logical_not(np.all(np.isfinite(mesh.vertices), axis=1));
    bad_vertex_indices = np.arange(mesh.num_vertices, dtype=int)[bad_vertex];
    for i in bad_vertex_indices:
        adj_v = mesh.get_vertex_adjacent_vertices(i);
        adj_v = adj_v[np.logical_not(bad_vertex[adj_v])];
        vertices[i] = np.mean(vertices[adj_v,:], axis=0);

    out_mesh = pymesh.form_mesh(vertices, mesh.faces);

    if mesh.has_attribute("corner_texture"):
        faces = mesh.faces;
        uv = np.copy(mesh.get_attribute("corner_texture").reshape((-1, 2)));
        bad_uv = np.logical_not(np.all(np.isfinite(uv), axis=1));
        bad_uv_indices = np.arange(len(uv), dtype=int)[bad_uv];
        for i in bad_uv_indices:
            fi = i // mesh.vertex_per_face;
            ci = i % mesh.vertex_per_face;
            vi = faces[fi, ci];
            adj_f = mesh.get_vertex_adjacent_faces(vi);
            adj_c = [adj_f[j]*mesh.vertex_per_face +
                    np.argwhere(f==vi).ravel()[0]
                    for j,f in enumerate(faces[adj_f, :])];
            adj_c = np.array(adj_c, dtype=int);
            adj_c = adj_c[np.logical_not(bad_uv[adj_c])];
            if len(adj_c) > 0:
                uv[i] = np.mean(uv[adj_c,:], axis=0);
            else:
                # All corner uv at this vertex is NaN. Average the one-ring instead.
                adj_c = [adj_f[j]*mesh.vertex_per_face +
                        (np.argwhere(f==vi).ravel()[0]+1)%mesh.vertex_per_face
                        for j,f in enumerate(faces[adj_f, :])];
                adj_c += [adj_f[j]*mesh.vertex_per_face +
                        (np.argwhere(f==vi).ravel()[0]+2)%mesh.vertex_per_face
                        for j,f in enumerate(faces[adj_f, :])];
                adj_c = np.array(adj_c, dtype=int);
                adj_c = adj_c[np.logical_not(bad_uv[adj_c])];
                uv[i] = np.mean(uv[adj_c,:], axis=0);

        out_mesh.add_attribute("corner_texture");
        out_mesh.set_attribute("corner_texture", uv);

    pymesh.save_mesh(args.output_mesh, out_mesh);

if __name__ == "__main__":
    main();
