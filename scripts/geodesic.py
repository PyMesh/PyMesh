#!/usr/bin/env python

""" Inplementation of geodesic in heat.  See 
http://www.cs.cmu.edu/~kmcrane/Projects/GeodesicsInHeat/
for more details.
"""

import argparse
import pymesh
import numpy as np
from numpy.linalg import norm
import scipy.sparse
import scipy.sparse.linalg

def parse_args():
    parser = argparse.ArgumentParser(description=__doc__);
    parser.add_argument("--source", type=int, default=0,
            help="source vertex index");
    parser.add_argument("input_mesh");
    parser.add_argument("output_mesh");
    return parser.parse_args();

def compute_edge_field(mesh):
    edges = [];
    vertices = mesh.vertices;
    for f in mesh.faces:
        e = vertices[f[[1,2,0]]] - vertices[f]
        edges.append(e);
    mesh.add_attribute("edges");
    mesh.set_attribute("edges", np.ravel(edges));

def compute_cotan_field(mesh):
    cotans = [];
    edges = mesh.get_attribute("edges").reshape((-1, 3));
    for i,f in enumerate(mesh.faces):
        e0 = edges[i*3];
        e1 = edges[i*3+1];
        e2 = edges[i*3+2];
        c0 = np.dot(e0, -e2) / norm(np.cross(e0, -e2));
        c1 = np.dot(e1, -e0) / norm(np.cross(e1, -e0));
        c2 = np.dot(e2, -e1) / norm(np.cross(e2, -e1));
        cotans.append(c0);
        cotans.append(c1);
        cotans.append(c2);
    mesh.add_attribute("cotan");
    mesh.set_attribute("cotan", np.ravel(cotans));

def compute_area_and_normal_field(mesh):
    mesh.add_attribute("face_area");
    mesh.add_attribute("vertex_area");
    mesh.add_attribute("face_normal");

def assemble_laplacian_matrix(mesh):
    i = [];
    j = [];
    data = [];
    cotans = mesh.get_attribute("cotan").reshape((-1, 3));
    for fi,f in enumerate(mesh.faces):
        for curr_i in range(3):
            next_i = (curr_i+1) % 3;
            prev_i = (curr_i+2) % 3;

            i.append(f[curr_i]);
            j.append(f[curr_i]);
            data.append((-cotans[fi, next_i]-cotans[fi, prev_i])/2.0);

            i.append(f[curr_i]);
            j.append(f[next_i]);
            data.append(cotans[fi, prev_i] / 2.0);

            i.append(f[curr_i]);
            j.append(f[prev_i]);
            data.append(cotans[fi, next_i] / 2.0);

    L = scipy.sparse.coo_matrix((data, (i,j)), shape=(mesh.num_vertices,
        mesh.num_vertices)).tocsc();
    return L;

def assemble_mass_matrix(mesh):
    area = mesh.get_attribute("vertex_area").ravel();
    M = scipy.sparse.dia_matrix((area, (0)), shape=(mesh.num_vertices,
        mesh.num_vertices));
    return M;

def compute_gradient(mesh, u):
    area = mesh.get_attribute("face_area").ravel();
    normal = mesh.get_attribute("face_normal").reshape((-1, 3));
    edges = mesh.get_attribute("edges").reshape((-1, 3, 3));
    grad = [];

    for fi,f in enumerate(mesh.faces):
        A = area[fi];
        N = normal[fi];
        e0 = edges[fi, 0];
        e1 = edges[fi, 1];
        e2 = edges[fi, 2];
        u0 = u[f[0]];
        u1 = u[f[1]];
        u2 = u[f[2]];
        grad_u = (np.cross(N, e0) * u2 +
                np.cross(N, e1) * u0 +
                np.cross(N, e2) * u1) / (2 * A);
        grad.append(grad_u);
    return np.array(grad);

def compute_divergence(mesh, X):
    cotans = mesh.get_attribute("cotan").reshape((-1, 3));
    edges = mesh.get_attribute("edges").reshape((-1, 3, 3));
    div_X = np.zeros(mesh.num_vertices);

    for fi,f in enumerate(mesh.faces):
        x_fi = X[fi];
        for i in range(3):
            j = (i+1) % 3;
            k = (i+2) % 3;
            div_X[f[i]] += cotans[fi, k] * np.dot(x_fi, edges[fi, i]) / 2.0;
            div_X[f[i]] += cotans[fi, j] * np.dot(x_fi,-edges[fi, k]) / 2.0;

    return div_X;

def main():
    args = parse_args();
    mesh = pymesh.load_mesh(args.input_mesh);
    assert(mesh.dim == 3);
    assert(mesh.vertex_per_face == 3);

    compute_edge_field(mesh);
    assert(mesh.has_attribute("edges"));
    compute_cotan_field(mesh);
    assert(mesh.has_attribute("cotan"));
    compute_area_and_normal_field(mesh);
    assert(mesh.has_attribute("vertex_area"));
    assert(mesh.has_attribute("face_normal"));

    L = assemble_laplacian_matrix(mesh);
    A = assemble_mass_matrix(mesh);

    t = np.mean(mesh.get_attribute("face_area").ravel());
    rhs = np.zeros(mesh.num_vertices);
    rhs[args.source] = 1.0;

    u = scipy.sparse.linalg.spsolve(A - t*L, rhs);
    mesh.add_attribute("u");
    mesh.set_attribute("u", u);

    grad_u = compute_gradient(mesh, u);
    mesh.add_attribute("grad_u");
    mesh.set_attribute("grad_u", grad_u.ravel());

    X = -grad_u / norm(grad_u, axis=1)[...,np.newaxis];
    mesh.add_attribute("X");
    mesh.set_attribute("X", X.ravel());

    div_X = compute_divergence(mesh, X);
    mesh.add_attribute("div_X");
    mesh.set_attribute("div_X", div_X);

    phi = scipy.sparse.linalg.spsolve(L, div_X);
    phi = phi - np.amin(phi);
    mesh.add_attribute("phi");
    mesh.set_attribute("phi", phi);

    pymesh.save_mesh(args.output_mesh, mesh, "u", "grad_u", "X", "div_X", "phi");


if __name__ == "__main__":
    main();
